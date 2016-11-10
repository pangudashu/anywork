package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"rtree/rtree"
	"strconv"
)

const (
	DIMENSION = 2
)

type Point struct {
	Lat float64 `json:"lat"`
	Lng float64 `json:"lng"`
}

type Resp struct {
	Errno int         `json:"errno"`
	Data  interface{} `json:"data"`
}

var point_data_path = "./point.data"
var point_list map[string]*Point
var root_node *rtree.Node

func main() {
	rtree.DIM = DIMENSION
	rtree.SetUnitSphereVolume(2)

	initPoint()
	initRTree()

	initHttpServer()
}

func searchHandler(w http.ResponseWriter, r *http.Request) {
	r.ParseForm()

	_lat := r.Form.Get("lat")
	_lng := r.Form.Get("lng")
	_r := r.Form.Get("r")

	if _lat == "" || _lng == "" {
		w.Write([]byte(`{"errno":0,"data":[]}`))
		return
	}

	radius := 100.0

	if _r != "" {
		radius, _ = strconv.ParseFloat(_r, 64)
	}

	area := (0.32 / 40000) * radius

	lat, _ := strconv.ParseFloat(_lat, 64)
	lng, _ := strconv.ParseFloat(_lng, 64)

	search_rect := &rtree.Rect{
		Boundary: [][2]float64{{lat - area, lat + area}, {lng - area, lng + area}},
	}

	ret := make([]*rtree.Rect, 0)
	search_cnt := 0
	rtree.Search(root_node, search_rect, &ret, &search_cnt)

	res := make([]*Point, len(ret))

	for i, v := range ret {
		res[i] = point_list[v.Data.(string)]
	}

	resp := &Resp{
		Errno: 0,
		Data:  res,
	}

	res_byte, _ := json.Marshal(resp)

	w.Write(res_byte)
}

func listHandler(w http.ResponseWriter, r *http.Request) {
	resp := &Resp{
		Errno: 0,
		Data:  point_list,
	}

	res_byte, _ := json.Marshal(resp)
	w.Write(res_byte)
}

func indexHandler(w http.ResponseWriter, r *http.Request) {
	http.ServeFile(w, r, "./web/index.html")
}

func initHttpServer() {
	http.HandleFunc("/index", indexHandler)
	http.HandleFunc("/search", searchHandler)
	http.HandleFunc("/list", listHandler)

	fmt.Println(">>>initHttpServer success")

	http.ListenAndServe(":9999", nil)
}

func initRTree() {
	root_node = rtree.NewNode()

	for k, p := range point_list {
		r := &rtree.Rect{
			Boundary: [][2]float64{{p.Lat, p.Lat + 0.000001}, {p.Lng, p.Lng + 0.000001}},
			Data:     k,
		}
		root_node = rtree.Insert(root_node, r)
	}
	fmt.Println(">>>initRTree success")
}

func initPoint() {
	file, err := os.Open(point_data_path)
	if err != nil {
		return
	}

	d, _ := ioutil.ReadAll(file)
	list := bytes.Split(d, []byte("\n"))

	point_list = make(map[string]*Point, len(list))

	for _, v := range list {
		fields := bytes.Split(v, []byte(","))
		if len(fields) < 3 {
			continue
		}
		_lat, _ := strconv.ParseFloat(string(fields[1]), 64)
		_lng, _ := strconv.ParseFloat(string(fields[2]), 64)
		p := &Point{
			Lat: _lat,
			Lng: _lng,
		}

		point_list[string(fields[0])] = p
	}

	fmt.Println(">>>initPoint success")
}

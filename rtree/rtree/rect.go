package rtree

import (
	"math"
)

//矩形
type Rect struct {
	Boundary [][2]float64 //边界
	Data     interface{}
}

var (
	DIM = 2 //维
)

var UnitSphereVolume float64

func SetDim(dim int) {
	DIM = dim
}

func SetUnitSphereVolume(dimension int) {
	log_pi := math.Log(3.1415926535)

	log_gamma := math.Gamma(float64(dimension)/2.0 + 1)

	log_volume := float64(dimension)/2.0*log_pi - log_gamma

	UnitSphereVolume = math.Exp(log_volume)

	switch dimension {
	case 2:
		UnitSphereVolume = 3.141593
	case 3:
		UnitSphereVolume = 4.188790
	case 4:
		UnitSphereVolume = 4.934802
	}
}

//计算体积
func GetSphereVolum(rect *Rect) float64 {
	sum_of_squares := 0.0

	for _, b := range rect.Boundary {
		half_extent := b[1] - b[0]
		sum_of_squares += half_extent * half_extent
	}
	radius := math.Sqrt(sum_of_squares)
	return math.Pow(radius, float64(DIM)) * UnitSphereVolume
}

//合并两个矩形
func CombineRect(r1, r2 *Rect) *Rect {
	new_rect := &Rect{}
	new_rect.Boundary = make([][2]float64, DIM)

	for dim := 0; dim < DIM; dim++ {
		new_rect.Boundary[dim][0] = math.Min(r1.Boundary[dim][0], r2.Boundary[dim][0])
		new_rect.Boundary[dim][1] = math.Max(r1.Boundary[dim][1], r2.Boundary[dim][1])
	}
	return new_rect
}

func CheckOverlap(r1, r2 *Rect) bool {
	for dim := 0; dim < DIM; dim++ {
		if r1.Boundary[dim][0] > r2.Boundary[dim][1] || r2.Boundary[dim][0] > r1.Boundary[dim][1] {
			return false
		}
	}
	return true
}

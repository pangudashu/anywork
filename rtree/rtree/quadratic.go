package rtree

import (
//"fmt"
)

type PartitionVars struct {
	partition []int      //分类数组：下标为buffer索引，value为分组key
	total     int        //buffer长度
	min       int        //每个分组最小数量
	taken     []bool     //buffer已分组标识
	count     [2]int     //各分组已选数量
	cover     [2]*Rect   //分组现有大小
	area      [2]float64 //分组当前面积
}

func pickSeed(p *PartitionVars, buffer []*Branch) {
	buffer_len := len(buffer)
	least_upper := make([]int, DIM)    //各维度最小上限
	greatest_lower := make([]int, DIM) //各维度最大下限
	width := make([]float64, DIM)

	//get cover buffer rect
	cover_rect := buffer[0].CoverRect
	for j := 1; j < buffer_len; j++ {
		cover_rect = CombineRect(cover_rect, buffer[j].CoverRect)
	}

	for dim := 0; dim < DIM; dim++ {
		greatest_lower[dim] = 0
		least_upper[dim] = 0

		for i := 1; i < buffer_len; i++ {
			if buffer[i].CoverRect.Boundary[dim][0] > buffer[greatest_lower[dim]].CoverRect.Boundary[dim][0] {
				greatest_lower[dim] = i
			}
			if buffer[i].CoverRect.Boundary[dim][1] < buffer[least_upper[dim]].CoverRect.Boundary[dim][1] {
				least_upper[dim] = i
			}
		}

		width[dim] = cover_rect.Boundary[dim][1] - cover_rect.Boundary[dim][0]
	}

	var w, separation, best_sep float64
	var seed0, seed1 int

	for dim := 0; dim < DIM; dim++ {
		if width[dim] == 0 {
			w = 1
		} else {
			w = width[dim]
		}

		rect_low := buffer[least_upper[dim]].CoverRect
		rect_high := buffer[greatest_lower[dim]].CoverRect

		separation = (rect_high.Boundary[dim][0] - rect_low.Boundary[dim][1]) / w
		if dim == 0 {
			seed0 = least_upper[dim]
			seed1 = greatest_lower[dim]
			best_sep = separation
		} else {
			if separation <= best_sep {
				continue
			}
			seed0 = least_upper[dim]
			seed1 = greatest_lower[dim]
			best_sep = separation
		}
	}
	if seed0 != seed1 {
		setPartition(p, buffer, seed0, 0)
		setPartition(p, buffer, seed1, 1)
	}
}

func pickNext(p *PartitionVars, buffer []*Branch) {
	new_cover_rect := make([]*Rect, 2)
	new_area := make([]float64, 2)
	increase := make([]float64, 2)

	for i := 0; i < p.total; i++ {
		if p.taken[i] == true {
			continue
		}

		//如果一个组中现有的条目太少，以至于剩余未分配的所有条目都要分配到这个组当中，才能避免下溢，则将剩余的所有条目都分配到这个组中
		if p.count[0] >= p.total-p.min {
			setPartition(p, buffer, i, 1)
			continue
		} else if p.count[1] >= p.total-p.min {
			setPartition(p, buffer, i, 0)
			continue
		}

		for group := 0; group < 2; group++ {
			if p.count[group] == 0 {
				new_cover_rect[group] = buffer[i].CoverRect
			} else {
				new_cover_rect[group] = CombineRect(p.cover[group], buffer[i].CoverRect)
			}
			new_area[group] = GetSphereVolum(new_cover_rect[group]) //计算rect分别加入两个分组后的面积
			increase[group] = new_area[group] - p.area[group]       //新分组的面积增长
		}

		if increase[0] < increase[1] { //优先放入面积增长最小的一组
			setPartition(p, buffer, i, 0)
		} else if increase[1] < increase[0] {
			setPartition(p, buffer, i, 1)
		} else if p.area[0] < p.area[1] { //若面积增长相同，则放入面积较小的一组
			setPartition(p, buffer, i, 0)
		} else if p.area[1] < p.area[0] {
			setPartition(p, buffer, i, 1)
		} else if p.count[0] < p.count[1] { //若面积也相同，则选择数量较少的一组
			setPartition(p, buffer, i, 0)
		} else {
			setPartition(p, buffer, i, 1)
		}
	}
}

func classify(p *PartitionVars, buffer []*Branch, n1 *Node, n2 *Node) {
	for i := 0; i < p.total; i++ {
		switch p.partition[i] {
		case 0:
			AddBranch(n1, buffer[i])
		case 1:
			AddBranch(n2, buffer[i])
		}
	}
}

//设置分组
func setPartition(p *PartitionVars, buffer []*Branch, index int, group int) {
	p.partition[index] = group
	p.taken[index] = true

	if p.count[group] == 0 {
		p.cover[group] = buffer[index].CoverRect
	} else {
		p.cover[group] = CombineRect(buffer[index].CoverRect, p.cover[group])
	}
	p.area[group] = GetSphereVolum(p.cover[group])
	p.count[group]++
}

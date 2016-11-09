package rtree

import (
//"fmt"
)

func Insert(root *Node, rect *Rect) *Node {
	//create branch
	new_node := searchAndInsert(root, rect)
	if new_node != nil { //root split
		new_root := NewNode()
		new_root.Level = root.Level + 1

		b1 := &Branch{CoverRect: nodeCover(root), Child: root}
		b2 := &Branch{CoverRect: nodeCover(new_node), Child: new_node}

		AddBranch(new_root, b1)
		AddBranch(new_root, b2)

		return new_root
	}

	return root
}

func Search(root *Node, rect *Rect, ret *[]*Rect, search_cnt *int) {
	if root.Level > 0 { //非叶子节点
		for i := 0; i < MAXCARD; i++ {
			(*search_cnt)++
			if root.BranchList[i] != nil && root.BranchList[i].Child != nil && CheckOverlap(rect, root.BranchList[i].CoverRect) {
				Search(root.BranchList[i].Child, rect, ret, search_cnt)
			}
		}
	} else { //叶子节点
		for i := 0; i < MAXCARD; i++ {
			(*search_cnt)++
			if root.BranchList[i] == nil || false == CheckOverlap(rect, root.BranchList[i].CoverRect) {
				continue
			}
			//find
			*ret = append(*ret, root.BranchList[i].CoverRect)
		}
	}
}

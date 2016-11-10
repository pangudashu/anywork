package rtree

import (
	"math"
)

var (
	MAXCARD = 3
)

//r-tree节点
type Node struct {
	BranchCnt  int
	Level      int
	BranchList []*Branch
}

//node分支
type Branch struct {
	CoverRect *Rect
	Child     *Node
}

func NewNode() *Node {
	n := &Node{
		BranchList: make([]*Branch, MAXCARD),
	}

	return n
}

//设置每个节点的最大条目数
//非叶子非根节点的条数：ceil(MAXCARD/2) <= m MAXCARD
func SetNodeMaxCard(card int) {
	MAXCARD = card
}

//most important!!!!
func searchAndInsert(n *Node, rect *Rect) (new_node *Node) {
	if n.Level > 0 {
		i := PickBranch(n, rect) //找一个branch迭代插入
		//选择child插入
		new_node = searchAndInsert(n.BranchList[i].Child, rect)
		if new_node == nil { //节点未分裂
			//调整外廓rect
			n.BranchList[i].CoverRect = CombineRect(n.BranchList[i].CoverRect, rect)
			return nil
		} else { //节点分裂
			n.BranchList[i].CoverRect = nodeCover(n.BranchList[i].Child)
			//将新分裂出的节点选择一个branch插入
			new_branch := &Branch{CoverRect: nodeCover(new_node), Child: new_node}
			return AddBranch(n, new_branch)
		}
	} else if n.Level == 0 {
		new_branch := &Branch{
			CoverRect: rect,
			Child:     nil,
		}
		return AddBranch(n, new_branch)
	}

	return nil
}

/**
 * 找最佳branch
 * 标准：选择rect插入后外廓扩张最小的branch，如果有两个branch在rect插入后外廓扩张相同，则选择面积较小的branch插入
 */
func PickBranch(n *Node, rect *Rect) int {
	is_start := true
	best := 0
	min_increase := 0.0
	min_area := 0.0

	for i := 0; i < MAXCARD; i++ {
		if n.BranchList[i] == nil || n.BranchList[i].Child == nil {
			continue
		}

		area := GetSphereVolum(n.BranchList[i].CoverRect)
		tmp_rect := CombineRect(n.BranchList[i].CoverRect, rect)
		increase := GetSphereVolum(tmp_rect) - area //rect合并后增长的面积

		if is_start || increase < min_increase {
			best = i
			min_increase = increase
			min_area = area
			is_start = false
		} else if increase == min_increase && area < min_area {
			best = i
			min_increase = increase
			min_area = area
		}
	}

	return best
}

func AddBranch(n *Node, b *Branch) (new_node *Node) {
	if n.BranchCnt < MAXCARD {
		for i := 0; i < MAXCARD; i++ {
			if n.BranchList[i] != nil {
				continue
			}
			n.BranchCnt++
			n.BranchList[i] = b

			return nil
		}
		return nil
	}
	return SplitNode(n, b)
}

//将节点n分裂为n1、n2
func SplitNode(n *Node, new_branch *Branch) (new_node *Node) {
	buffer := make([]*Branch, MAXCARD+1)
	for i, v := range n.BranchList {
		buffer[i] = v
	}
	buffer[MAXCARD] = new_branch
	//new_node := &Node{}
	partition := &PartitionVars{
		partition: make([]int, MAXCARD+1),
		total:     MAXCARD + 1,
		min:       int(math.Ceil(float64(MAXCARD) / 2.0)),
		taken:     make([]bool, MAXCARD+1),
	}

	new_node = NewNode()
	new_node.Level = n.Level

	pickSeed(partition, buffer) //为两个组选择第一个条目
	pickNext(partition, buffer)
	n.BranchCnt = 0
	n.BranchList = make([]*Branch, MAXCARD)
	classify(partition, buffer, n, new_node) //分配到分裂后的新节点

	return new_node
}

func nodeCover(n *Node) *Rect {
	r := &Rect{}
	is_start := true
	for i := 0; i < MAXCARD; i++ {
		if n.BranchList[i] == nil {
			continue
		}
		if is_start {
			r = n.BranchList[i].CoverRect
			is_start = false
			continue
		}
		r = CombineRect(r, n.BranchList[i].CoverRect)
	}
	return r
}

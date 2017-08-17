package routine_pool

import (
	"errors"
	"log"
	"sync"
)

type Pool struct {
	name       string
	mode       int
	taskQueue  chan *task
	workerPool map[*worker]*int
	taskPool   *sync.Pool
	m          *master
	finishCh   chan bool //recive the result of workers quit

	initNum      int
	maxNum       int
	minSpareNum  int
	maxSpareNum  int
	maxSpareTime int

	queueLen int
	isBlock  bool
}

//manage mode
const (
	MODE_STATIC   = 1
	MODE_DYNAMIC  = 2
	MODE_ONDEMOND = 3
)

func NewPool(name string) *Pool {
	return &Pool{name: name}
}

//set manage mode：
//1)static: create initNum routines, and never kill them
//2)dynamic: create initNum routines on startup, and keep the num of idle routine between minSpareNum and maxSpareNum
//3)ondemond: no routine created on startup, and create one when task comming
func (p *Pool) SetMode(mode, initNum, maxNum, minSpareNum, maxSpareNum, maxSpareTime int) error {
	switch mode {
	case MODE_STATIC:
		if initNum == 0 {
			return errors.New("initNum must greater than zero for static mode")
		}
	case MODE_DYNAMIC:
		if maxSpareNum < minSpareNum {
			return errors.New("maxSpareNum must greater than minSpareNum for dynamic mode")
		}
	case MODE_ONDEMOND:
	default:
		return errors.New("Unkown Mode")
	}

	p.mode = mode
	p.initNum = initNum
	p.maxNum = maxNum
	p.minSpareNum = minSpareNum
	p.maxSpareNum = maxSpareNum
	p.maxSpareTime = maxSpareTime

	return nil
}

func (p *Pool) SetQueue(len int, isBlock bool) {
	p.queueLen = len
	p.isBlock = isBlock
}

//startup
func (p *Pool) StartUp() error {
	if p.queueLen <= 0 {
		return errors.New("You must set the task queue length by SetQueue() befor startup and the length must greater than zero")
	}

	switch p.mode {
	case MODE_STATIC:
	case MODE_DYNAMIC:
	case MODE_ONDEMOND:
	default:
		return errors.New("You must choose one mode by SetMode() befor startup")
	}
	//1) init task queue
	p.taskQueue = make(chan *task, p.queueLen)
	p.taskPool = &sync.Pool{
		New: func() interface{} { return new(task) },
	}

	//2) init routine
	p.workerPool = make(map[*worker]*int, p.initNum)
	switch p.mode {
	case MODE_STATIC, MODE_DYNAMIC:
		for i := 0; i < p.initNum; i++ {
			w := newWorker(p)
			p.workerPool[w] = &w.status
		}
	case MODE_ONDEMOND: //nothing todo
	}

	//3) startup master routine
	newMaster(p)

	p.finishCh = make(chan bool, 1)

	return nil
}

//block
func (p *Pool) Close() {
	p.m.quitCh <- true

	//block
	<-p.finishCh
}

type task struct {
	arg     interface{}
	handler func(interface{})
}

//register new task
func (p *Pool) RegTask(handler func(interface{}), arg interface{}) error {
	if handler == nil {
		return errors.New("The handler of registered task can't be nil")
	}

	t := p.taskPool.Get().(*task)
	t.arg = arg
	t.handler = handler

	select {
	case p.taskQueue <- t:
	default:
		//it's not a good thing
		log.Print("task queue is full!")
		go func() {
			p.taskQueue <- t
		}()
	}
	if p.mode == MODE_ONDEMOND {
		select {
		case p.m.createCh <- true:
		default:
		}
	}
	return nil
}

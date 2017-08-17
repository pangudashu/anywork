package routine_pool

import (
	"log"
	"sync"
	"time"
)

type worker struct {
	pool     *Pool
	status   int       //worker status
	quitCh   chan bool //recive QUIT notice from master
	idleTime int64
	mu       sync.Mutex
}

//worker status
const (
	WORKER_STATUS_BUSY = iota
	WORKER_STATUS_IDLE
	WORKER_STATUS_QUIT
)

func newWorker(pool *Pool) *worker {
	w := &worker{
		pool:   pool,
		quitCh: make(chan bool),
	}
	go w.cycle()
	return w
}

func (w *worker) cycle() {
	defer func() {
		if err := recover(); err != nil {
			log.Print(err)
			//worker will quit out
			w.status = WORKER_STATUS_QUIT
		}
	}()

	for {
		select {
		case task := <-w.pool.taskQueue: //wait for new task
			//1) set worker status
			w.status = WORKER_STATUS_BUSY

			//2) call handler
			task.handler(task.arg)

			//3) work done
			//w.pool.taskPool.Put(task)
			if len(w.pool.taskQueue) == 0 {
				w.status = WORKER_STATUS_IDLE
				w.idleTime = time.Now().Unix()
			}
		case <-w.quitCh: //will quit
			log.Print("worker quit out")
			w.status = WORKER_STATUS_QUIT
			return
		case <-time.After(time.Second * 1):
			if w.status != WORKER_STATUS_IDLE {
				w.status = WORKER_STATUS_IDLE
				w.idleTime = time.Now().Unix()
			}

			log.Print("worker is idle")
		}
	}
}

func (w *worker) kill() {
	w.quitCh <- true
}

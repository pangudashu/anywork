package routine_pool

import (
	"log"
	"time"
)

type master struct {
	pool       *Pool
	quitCh     chan bool
	createCh   chan bool
	createRate int
}

const (
	MAX_CREATE_RATE = 2 << 5
)

func newMaster(p *Pool) *master {
	m := &master{
		pool:       p,
		quitCh:     make(chan bool, 1),
		createCh:   make(chan bool, p.maxNum),
		createRate: 1,
	}
	p.m = m

	//startup master
	go m.start()

	return m
}

func (m *master) start() {

	is_quit := false
	for {
		select {
		case <-m.quitCh:
			is_quit = true
		case <-m.createCh: //it's time to create worker,just for ondemond mode
			if len(m.pool.workerPool) < m.pool.maxNum {
				w := newWorker(m.pool)
				m.pool.workerPool[w] = &w.status
			}
		case <-time.After(time.Second * 1):
		}

		var last_idle_worker *worker
		idle_cnt := 0
		busy_cnt := 0
		for w, status := range m.pool.workerPool {
			switch *status {
			case WORKER_STATUS_QUIT:
				//remove quit out worker
				delete(m.pool.workerPool, w)
			case WORKER_STATUS_IDLE:
				idle_cnt++
				if (last_idle_worker == nil) || (last_idle_worker != nil && last_idle_worker.idleTime > w.idleTime) {
					last_idle_worker = w
				}
			case WORKER_STATUS_BUSY:
				busy_cnt++
			}
			//notice to all workers to quit
			if is_quit == true {
				w.kill()
			}
		}

		if is_quit == true {
			if len(m.pool.workerPool) == 0 {
				break
			} else {
				//wait for worker to quit
				continue
			}
		}

		curr_worker_num := len(m.pool.workerPool)
		switch m.pool.mode {
		case MODE_STATIC:
			if curr_worker_num == m.pool.initNum {
				break
			}
			//some worker have quit out
			for i := 0; i < m.pool.initNum-curr_worker_num; i++ {
				w := newWorker(m.pool)
				m.pool.workerPool[w] = &w.status
			}
		case MODE_DYNAMIC:
			if idle_cnt < m.pool.minSpareNum { //create more workers
				if curr_worker_num >= m.pool.maxNum {
					log.Print("The num of workers have reached maxNum, you need to increase maxNum")
					break
				}
				create_num := m.createRate
				if create_num > (m.pool.maxNum - curr_worker_num) {
					create_num = m.pool.maxNum - curr_worker_num
				}
				//create
				for i := 0; i < create_num; i++ {
					w := newWorker(m.pool)
					m.pool.workerPool[w] = &w.status
				}

				//speed up to create worker for the next loop
				if m.createRate < MAX_CREATE_RATE {
					m.createRate *= 2
				}
				break
			}
			if idle_cnt > m.pool.maxSpareNum { //kill some workers
				//just kill the last idle worker one time
				last_idle_worker.kill()
			}

			//the num of workers is balanced: reset create rate
			m.createRate = 1

		case MODE_ONDEMOND:
			//check idle time
			if last_idle_worker == nil {
				break
			}
			//if the idle time greater than maxSpareTime, then kill it
			if int(time.Now().Unix()-last_idle_worker.idleTime) > m.pool.maxSpareTime {
				//kill the last_idle_worker
				last_idle_worker.kill()
			}
		}

		log.Printf("master check...[pool name]%s [worker num]%d [busy/idle num]%d/%d [task queue]%d",
			m.pool.name,
			len(m.pool.workerPool),
			busy_cnt,
			idle_cnt,
			len(m.pool.taskQueue))
	}

	//master quit out
	m.pool.finishCh <- true
}

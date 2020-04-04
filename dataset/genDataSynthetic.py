#!/usr/bin/env python

import numpy as np
from random import randint, sample
import sys
import os
import commands

class constForDataSet:
	locRng = [0, 600]
	workerLambda = 2
	taskLambda = 20
	waitSigma = 10
	workerNumList = [100,200,300,400,500]
	taskNumList = [1000,2000,3000,4000,5000]
	workerWaitList = [30,60,90,120,150]
	taskWaitList = [60,120,180,240,300]
	scaleTaskList = [x*1000 for x in [10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500]]
	scaleWorkerList = [x*1000/taskLambda*workerLambda for x in [10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500]]
	utilityList = [2,4,6,8,10]
	defaultValues = [
		workerNumList[len(workerNumList)/2],
		taskNumList[len(taskNumList)/2],
		workerWaitList[len(workerWaitList)/2],
		taskWaitList[len(taskWaitList)/2],
		utilityList[len(utilityList)/2],
	]
	Tmax = 7200


class CFDS(constForDataSet):
	pass

class baseGenerator:

	def gen(self, n):
		pass

class worker_t:

	def __init__(self, sloc=[0,0], dloc=[0,0], releaseTime=-1, expireTime=-1):
		self.sloc = sloc
		self.dloc = dloc
		self.releaseTime = releaseTime
		self.expireTime = expireTime

	def __cmp__(self, other):
		return self.releaseTime < other.releaseTime

	def __lt__(self, other):
		return self.releaseTime < other.releaseTime

	def __str__(self):
		ret = "1 %s %s %s %s %s %s" % (self.sloc[0], self.sloc[1], self.dloc[0], self.dloc[1], self.releaseTime, self.expireTime)
		return ret

class task_t:

	def __init__(self, loc=[0,0], releaseTime=0, expireTime=0, ut=1):
		self.loc = loc
		self.rt = releaseTime
		self.et = expireTime
		self.ut = ut

	def __cmp__(self, other):
		return self.rt < other.rt

	def __lt__(self, other):
		return self.rt < other.rt

	def __str__(self):
		ret = "0 %s %s %s %s %s" % (self.loc[0], self.loc[1], self.rt, self.et, self.ut)
		return ret

class randomGenerator(baseGenerator):

	def __init__(self, mx):
		self.mx = mx

	def setMx(self, mx):
		self.mx = mx

	def gen(self, n):
		ret = [0] * n
		for i in xrange(n):
			x = randint(1, self.mx)
			ret[i] = x
		return ret

class normalGenerator(baseGenerator):

	def __init__(self, mu, sigma):
		self.mu = mu
		self.sigma = sigma

	def gen(self, n, lb = None, rb = None):
		# print lb, rb
		ret = np.random.normal(self.mu, self.sigma, n)
		for i in xrange(n):
			if lb is not None and ret[i]<lb:
				ret[i] = lb
			if rb is not None and ret[i]>rb:
				ret[i] = rb
		# print ret
		return ret

	def setMu(self, mu):
		self.mu = mu

	def setSigma(self, sigma):
		self.sigma = sigma


class uniformGenerator(baseGenerator):

	def __init__(self, low, high):
		self.low = low
		self.high = high

	def gen(self, n, lb = None, rb = None):
		ret = np.random.uniform(self.low, self.high, n)
		for i in xrange(n):
			if lb is not None and ret[i]<lb:
				ret[i] = lb
			if rb is not None and ret[i]>rb:
				ret[i] = rb
		return ret

	def setLow(self, low):
		self.low = low

	def setHigh(self, high):
		self.high = high

class expGenerator(baseGenerator):

	def __init__(self, mu):
		self.mu = mu

	def gen(self, n, lb = None, rb = None):
		ret = np.random.exponential(self.mu, n)
		for i in xrange(n):
			if lb is not None and ret[i]<lb:
				ret[i] = lb
			if rb is not None and ret[i]>rb:
				ret[i] = rb
		return ret

	def setMu(self, mu):
		self.mu = mu


def genLoc(n, low, high, samePermit=True):
	ret = [[0,0] for i in xrange(n)]
	if samePermit:
		for i in xrange(n):
			x = randint(low, high)
			y = randint(low, high)
			ret[i] = [x, y]
	else:
		st = set()
		for i in xrange(n):
			while True:
				x = randint(low, high)
				y = randint(low, high)
				t = (x, y)
				if t not in st:
					break
			ret[i] = [t[0], t[1]]
			st.add(t)
	return ret


def Length(a, b):
	ret = ((a[0]-b[0])**2 + (a[1]-b[1])**2) ** 0.5
	return int(ret + 1)


def genWorkers(n, sloc, dloc, sw, dew):
	ret = [worker_t() for i in xrange(n)]
	for i in xrange(n):
		ret[i].sloc = sloc[i]
		ret[i].dloc = dloc[i]
		ret[i].releaseTime = int(sw[i])
		ret[i].expireTime = int(ret[i].releaseTime + dew[i] + Length(sloc[i], dloc[i]))
	# ret.sort()
	return ret


def genTasks(n, loc, rt, det, ut):
	ret = [task_t() for i in xrange(n)]
	for i in xrange(n):
		ret[i].loc = loc[i]
		ret[i].rt = int(rt[i])
		ret[i].et = int(ret[i].rt + det[i])
		ret[i].ut = ut[i]
	# ret.sort()
	return ret


def genDataSet(desFileName, workers, tasks):
	with open(desFileName, "w") as fout:
		workerN = len(workers)
		taskN = len(tasks)
		i,j = 0,0
		fout.write("%d %d\n" % (workerN, taskN))
		while i<workerN and j<taskN:
			if workers[i].releaseTime <= tasks[j].rt:
				line = str(workers[i])
				i += 1
			else:
				line = str(tasks[j])
				j += 1
			fout.write("%s\n" % (line))
		while i < workerN:
			line = str(workers[i])
			i += 1
			fout.write("%s\n" % (line))
		while j < taskN:
			line = str(tasks[j])
			j += 1
			fout.write("%s\n" % (line))


def genDataSetName(workerNum, taskNum, workerWait, taskWait, utility):
	return "%04d_%04d_%04d_%04d_%02d" % (workerNum, taskNum, workerWait, taskWait, utility)


def genReleaseTime(lam, n):
	cnts = list(np.random.poisson(lam, int(n/lam+1)))
	cnts += [lam] * (n/lam + 1)
	ret = [0] * n
	i,k = 0,0
	for c in cnts:
		i += 1
		for j in xrange(c):
			ret[k] = i
			k += 1
			if k >= n:
				break
		if k >= n:
			break
	return ret


def	batchDataSet(desFilePath, dataSetId):
	if not os.path.exists(desFilePath):
		os.mkdir(desFilePath)
	workerNum, taskNum, workerWait, taskWait, Umax = CFDS.defaultValues
	workerNumMax = CFDS.workerNumList[-1]
	taskNumMax = CFDS.taskNumList[-1]
	workerSrcLocList = genLoc(workerNumMax, CFDS.locRng[0], CFDS.locRng[1])
	workerDesLocList = genLoc(workerNumMax, CFDS.locRng[0], CFDS.locRng[1])
	taskLocList = genLoc(taskNumMax, CFDS.locRng[0], CFDS.locRng[1])
	workerReleaseList = genReleaseTime(CFDS.workerLambda, workerNumMax)
	taskReleaseList = genReleaseTime(CFDS.taskLambda, taskNumMax)
	workerWaitList = normalGenerator(workerWait, CFDS.waitSigma).gen(workerNumMax, 0, max(CFDS.workerWaitList)*2)
	taskWaitList = normalGenerator(taskWait, CFDS.waitSigma).gen(taskNumMax, 0, max(CFDS.taskWaitList)*2)
	utilityList = randomGenerator(Umax).gen(taskNumMax)
	workerReleaseList.sort()
	taskReleaseList.sort()


	# varying of workerNum
	for workerNum in CFDS.workerNumList:
		workers = genWorkers(workerNum, workerSrcLocList[:workerNum], workerDesLocList[:workerNum], workerReleaseList[:workerNum], workerWaitList[:workerNum])
		tasks = genTasks(taskNum, taskLocList[:taskNum], taskReleaseList[:taskNum], taskWaitList[:taskNum], utilityList[:taskNum])
		tmpFilePath = genDataSetName(workerNum, taskNum, workerWait, taskWait, Umax)
		tmpFilePath = os.path.join(desFilePath, tmpFilePath)
		if not os.path.exists(tmpFilePath):
			os.mkdir(tmpFilePath)
		desFileName = "data_%02d.txt" % (dataSetId)
		desFileName = os.path.join(tmpFilePath, desFileName)
		if os.path.exists(desFileName):
			continue
		genDataSet(desFileName, workers, tasks)
	workerNum, taskNum, workerWait, taskWait, Umax = CFDS.defaultValues


	# varying of taskNum
	for taskNum in CFDS.taskNumList:
		workers = genWorkers(workerNum, workerSrcLocList[:workerNum], workerDesLocList[:workerNum], workerReleaseList[:workerNum], workerWaitList[:workerNum])
		tasks = genTasks(taskNum, taskLocList[:taskNum], taskReleaseList[:taskNum], taskWaitList[:taskNum], utilityList[:taskNum])
		tmpFilePath = genDataSetName(workerNum, taskNum, workerWait, taskWait, Umax)
		tmpFilePath = os.path.join(desFilePath, tmpFilePath)
		if not os.path.exists(tmpFilePath):
			os.mkdir(tmpFilePath)
		desFileName = "data_%02d.txt" % (dataSetId)
		desFileName = os.path.join(tmpFilePath, desFileName)
		if os.path.exists(desFileName):
			continue
		genDataSet(desFileName, workers, tasks)
	workerNum, taskNum, workerWait, taskWait, Umax = CFDS.defaultValues


	# varying of workerWait
	for workerWait in CFDS.workerWaitList:
		tmp_workerWaitList = normalGenerator(workerWait, CFDS.waitSigma).gen(workerNum, 0, max(CFDS.workerWaitList)*2)
		workers = genWorkers(workerNum, workerSrcLocList[:workerNum], workerDesLocList[:workerNum], workerReleaseList[:workerNum], tmp_workerWaitList)
		tasks = genTasks(taskNum, taskLocList[:taskNum], taskReleaseList[:taskNum], taskWaitList[:taskNum], utilityList[:taskNum])
		tmpFilePath = genDataSetName(workerNum, taskNum, workerWait, taskWait, Umax)
		tmpFilePath = os.path.join(desFilePath, tmpFilePath)
		if not os.path.exists(tmpFilePath):
			os.mkdir(tmpFilePath)
		desFileName = "data_%02d.txt" % (dataSetId)
		desFileName = os.path.join(tmpFilePath, desFileName)
		if os.path.exists(desFileName):
			continue
		genDataSet(desFileName, workers, tasks)
	workerNum, taskNum, workerWait, taskWait, Umax = CFDS.defaultValues


	# varying of taskWait
	for taskWait in CFDS.taskWaitList:
		tmp_taskWaitList = normalGenerator(taskWait, CFDS.waitSigma).gen(taskNum, 0, max(CFDS.taskWaitList)*2)
		workers = genWorkers(workerNum, workerSrcLocList[:workerNum], workerDesLocList[:workerNum], workerReleaseList[:workerNum], workerWaitList[:workerNum])
		tasks = genTasks(taskNum, taskLocList[:taskNum], taskReleaseList[:taskNum], tmp_taskWaitList[:taskNum], utilityList[:taskNum])
		tmpFilePath = genDataSetName(workerNum, taskNum, workerWait, taskWait, Umax)
		tmpFilePath = os.path.join(desFilePath, tmpFilePath)
		if not os.path.exists(tmpFilePath):
			os.mkdir(tmpFilePath)
		desFileName = "data_%02d.txt" % (dataSetId)
		desFileName = os.path.join(tmpFilePath, desFileName)
		if os.path.exists(desFileName):
			continue
		genDataSet(desFileName, workers, tasks)
	workerNum, taskNum, workerWait, taskWait, Umax = CFDS.defaultValues


	# varying of utility
	for Umax in CFDS.utilityList:
		tmp_utilityList = randomGenerator(Umax).gen(taskNum)
		workers = genWorkers(workerNum, workerSrcLocList[:workerNum], workerDesLocList[:workerNum], workerReleaseList[:workerNum], workerWaitList[:workerNum])
		tasks = genTasks(taskNum, taskLocList[:taskNum], taskReleaseList[:taskNum], taskWaitList[:taskNum], tmp_utilityList)
		tmpFilePath = genDataSetName(workerNum, taskNum, workerWait, taskWait, Umax)
		tmpFilePath = os.path.join(desFilePath, tmpFilePath)
		if not os.path.exists(tmpFilePath):
			os.mkdir(tmpFilePath)
		desFileName = "data_%02d.txt" % (dataSetId)
		desFileName = os.path.join(tmpFilePath, desFileName)
		if os.path.exists(desFileName):
			continue
		genDataSet(desFileName, workers, tasks)
	workerNum, taskNum, workerWait, taskWait, Umax = CFDS.defaultValues


def exp0(dataSetN = 30):
	desFilePath = "./synthetic"
	if not os.path.exists(desFilePath):
		os.mkdir(desFilePath)
	for dataSetId in xrange(dataSetN):
		batchDataSet(desFilePath, dataSetId)

if __name__ == "__main__":
	exp0(1)
	

SpatialCrowdsourcing-MWATP: Multi-Worker-Aware Task Planning in Real-Time Spatial Crowdsourcing
========================================================================

This repository stores the source code of the proposed solutions to the problem called MWATP in the following paper.

[1] **Multi-Worker-Aware Task Planning in Real-Time Spatial Crowdsourcing.**
*Qian Tao, Yuxiang Zeng, Zimu Zhou, Yongxin Tong, Lei Chen, Ke Xu.* DASFAA 2018: 301-317. [link](https://doi.org/10.1109/ICDE.2018.00037) 

If you find this work helpful in your research, please consider citing our paper and the bibtex are listed below:
```  
@inproceedings{DBLP:conf/dasfaa/TaoZZTC018,
  author    = {Qian Tao and
               Yuxiang Zeng and
               Zimu Zhou and
               Yongxin Tong and
               Lei Chen and
               Ke Xu},
  title     = {Multi-Worker-Aware Task Planning in Real-Time Spatial Crowdsourcing},
  booktitle = {{DASFAA}},
  pages     = {301--317},
  year      = {2018},
}
```  



Usage of the algorithms
---------------

### Environment

gcc/g++ version: 7.4.0 

OS: Ubuntu

### Compile the algorithms

cd algorithm && make all

ignore: the Delay-Planning algorithm in the paper

replan: the Fast-Planning algorithm in the paper


### Run the algorithms

./replan ../dataset/synthetic/0100_3000_0090_0180_06/data_00.txt

0100_3000_0090_0180_06: the varied paramters in the experiments

data_00.txt: the information of the tasks and workers (i.e., the input)

Description of the data generator
---------------

### Environment

Python: 2.7

### Run the scripts

genDataSynthetic.py: a script to generate the synthetic datasets in the experiments

synthetic: a sample of the synthetic datasets


Related other resources
------------------------
We have maintained a paper list of the studies on spatial crowdsourcing. [link](https://github.com/BUAA-BDA/SpatialCrowdsourcing-Survey)


Contact
------------
- Qian Tao: qiantao@buaa.edu.cn
- Yuxiang Zeng: yzengal@cse.ust.hk
- Yongxin Tong: yxtong@buaa.edu.cn


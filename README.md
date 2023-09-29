# Evil ELF 
## Created by Andrew Kosikowski, Daniel "Jun" Cho and Mabon Ninan
### Mentors: Dr. Boyang Wang and Dr. Anca Ralescu
---

## Reference
When reporting results that use the dataset or code in this repository, please cite the paper below:

*Andrew Kosikowski, Daniel (Jun) Cho, Mabon Ninan, Anca Ralescu, Boyang Wang “EvilELF: Evasion Attacks on Deep-Learning Malware Detection over ELF Files,” 22nd IEEE International Conference on Machine Learning and Applications (IEEE ICMLA 2023), Jacksonville, FL, December 15-17, 2023.*

**The dataset and code are for research purpose only**

---

## What is this?
The goal of this codebase is to modify existing malware without altering its functionality and investigate whether end-to-end deep learning malware detectors are capable of being fooled by the obfuscated malware. That is, disguising malware that is detected by your malware detector (or say your antivirus) and see if that disguise is enough to trick it. The goal of this project would be to expand upon previous work for Windows executables (PE files) and see if similar ideas and methods would apply for Linux executables (ELF files). Ultimately, these new malware samples would then be used to train new malware detectors to perform better against obfuscated malware in the future.


---

## Research Significance
You can find a detailed breakdown of some of this projects accomplishments in our paper. In it we propose multiple methods of evading detection from MalConv and other deep learning malware detectors and demonstrate we can successfully modify malware so that it evades detection. 

---

## Breakdown
The project is composed of three main groups: Analytics containing data, experiments, and results, MultiEvasion containing pre-existing malware detection code from a previous project (https://github.com/UCdasec/MultiEvasion), and ELFFileManipulator containing the novel modifications to ELF files in order to evade detection. The majority of this project was spent creating modifications for ELF files and contains a helpful library for others to modify ELF files themselves. Each main directory has a readme giving greater detail about its contents.


---
<!--
## Acknowledgements
This code was created as part of the Research Experiences for Undergraduates (REU) in Hardware and Embedded Systems Security and Trust (RHEST), NSF award 2150086, during the summer of 2023 at the University of Cincinnati. The project's code was created by Andrew Kosikowski and Daniel "Jun" Cho during their undergraduates at Rose-Hulman Institute of Technology and Hamilton College respectively. The project was mentored by Dr. Boyang Wang and Dr. Anca Ralescu at the University of Cincinnati. 
-->

## Contact
Andrew Kosikowski andkosikowski@gmail.com <!-- https://www.andrewkosikowski.com -->

Jun Cho junc13579@gmail.com

Mabon Ninan ninanmm@mail.uc.edu

Dr. Boyang Wang, boyang.wang@uc.edu

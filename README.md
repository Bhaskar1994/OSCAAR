# OSCAAR
Open Source Computational Aero-Acoustics Repository
Currently Under development.
Estimated time of release: July, 2017
This is an open-source project under-development. 
The main aim of this project will be implementing Farasaat's implementation of the FFOWCS-Williams Hawkings formulation for finding the aero-acoustic noise generated from flow.
This however won't include a flow solver. An user is expected to solve the flow using any solver comfortable to them. However as an initial basis, resultant flow data files of CGNS format will only be accepted.  
Unike most commercial codes, this code will give the user greater understanding of the acoustic source terms: monopoles, dipoles and quadropoles by listing and analyzing them. 
For this initial versions, the code will be only for subsonic flows (Mach number<0.98) . 
This code is meant only for instructional use, and not any commercial use. 
The computation of most of the partial derivatives are kept first order. An user is free to change that as per their requirement. 

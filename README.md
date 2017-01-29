# OSCAAR
Open Source Computational Aero-Acoustics Repository
This is an open-source project under-development. It's scheduled to be released in the next 3 months.
The main aim of this project will be implementing Farasaat's implementation of the FFOWCS-Williams Hawkings formulation for finding the aero-acoustic noise generated from flow.
This however won't include a flow solver. An user is expected to solve the flow using any solver comfortable to them. However as an initial basis, resultant flow data files of CGNS format will only be accepted. Other universal formmats(like HDF5) will be slowly covered, once the development of ver 1 will be over. 
Unike most commercial codes, this code will give the user greater understanding of the acoustic source terms: monopoles, dipoles and quadropoles by listing and analyzing them. 
For the initial versoins, the code will be only for subsonic flows (Mach number<0.98) . 
The code is developed mainly keeping subsonic fans, propellers and wind turbines in mind. 

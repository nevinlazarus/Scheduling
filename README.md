# Scheduling

<h1> Nevin's cool scheduling tool </h1>

Things you will need:  
g++ (c++11)  
make  

<h2> How to use: </h2>

Run make  
Execute ./Step input_filename people_per_shift (optional: leaders_per_shift)  
  
people_per_shift (including the shift leaders) > 0  
people_per_shift >= leaders_per_shift >= 0  

It will probably take a while depending on what params / input you have  

<h2> Input file </h2>  

Shift leaders should have "leader" in their name  
Input is in the below format  

Person A  
Person A's timeslot 1  
Person A's timeslot 2  
....  
Person B  
Person B's timeslot 1  
....  
end  
  
Example in cast.txt and input.txt  
  
<h2> Configuring: </h2>

Chucked in a few #defines so you can fine tune things a bit  
Check Step.cpp  

/**************************/
/* VAST Coding Challenge. */
/**************************/
* Foreword:
* This is a fully functional Simulation. :) I wrote this version here that you can run, and I wrote a second version in Unreal Engine 5.
* I'll be sending this project over along with the Unreal Engine 5 Build (I made it just for you) and a video of the build running if you want to view that instead.
* In the Unreal Engine 5 build, you can press Cntrl + C to enable the camera to fly around and use the W, A, S, and D keys to move around. FPS style.
* I have also built a small set of UI controls at the bottom on the screen in the build. Press Cntrl + C again to disable the camera and re-enable the mouse. This will allow you to click the buttons in the UI.
* Hope you enjoy, they're pretty cool. :)
* Now on to the Coding Challenge.

/*****************************/
/* Visual Studio C++ Project */
/*****************************/
The code in this project for the challenge was built in Pure C++. The source is contained in the Standard Visual Studio project structure. If you want you can view the source directly here on github, or clone the repository and run the simulation from Visual Studio.
There is very mininmal pseudo code, and the Simulation actually runs per the requirements laid out in the Coding Challenge document.
To configure the simulation to run in different "situations" change the values in: int main().
There, you will find a struct named "SimulationConfiguration" that structure houses the setup variables used to pre-configure the simulation before running.

/***********************/
/* TESTING STRATEGIES. */
/***********************/
* Strategy #1. Scale Test
* Usually scale would be considered when testing anything I develop. Scale in this case means Network bandwidth / latency, persistence size (non-volatile memory usage), and memory (volatile memory usage).
* But since this simulation runs locally the only considerations for scale I'm making are #1. The memory footprint of the simulation and #2. It's runtime performance (no crashes etc).

* Strategy #2. Ensure (through unit testing) when we add more Mining Trucks with same amount of Unloading Locations we experience global efficiency gains, while our efficiencies per truck would be expected to fall
* due to an increasing queue time. Unloading station efficiency should be maximized as they will be at full capacity at all times.

* Strategy #3. Ensure that when we reduce the number of Mining Trucks and increase the number of loading stations our efficiency per truck would be higher than in Strategy #1,
* while our unloading station efficiency would be reduced, as there are less trucks occupying a station at any given time - unloading Helium-3.

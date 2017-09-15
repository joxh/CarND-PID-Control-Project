# Reflections on the PID Project

## Parameter Description

> Describe the effect each of the P, I, D components had in your implementation.

* P-coefficient
  * The P coefficient determines how much the car will correct itself proportional to how far it is off course. This is a necessary coefficient, since it is the main driver of the correction. However, if there is any momentum in the system, use of only a P-coefficeint will lead to oscillations.
  * The D coefficient determines how much to compensate the correction because of improvements in the magnitude of the error. Dynamically it works a bit like a drag on the system and it contributes to the car straightening out as it approaches the center line instead of oscillating forever.
  * The I coefficient induces a correction proportional to the total accumulated (signed) error. This allows it to compensate for systematic forces which would otherwise lead to a persistent offset. In the case of the car, this can correspond to a long turn to the left. In the absense of this term, the car would persistantly miss to the right.

Unexpectedly, I noted that if the D-coefficient is made too large that it can induce oscillations too, as can the I-coefficient.

## Hyperparameter Choice

> Describe how the final hyperparameters were chosen.

I chose my hyperparameters by making an interface where I could change the parameters live while the car was on the track. This allowed me to manually adjust the hyperparameters in real time. 

By doing this I was able to steer the cars with the hyperparameters until it could steer itself:
  * I started with the coefficients all at 0. This corresponded to moving in a completely straight line.
  * When I noticed that the car was missing its turns or otherwise failing to correct at all, I increased the P-coefficient. By turning it up, I was able to get it to correct in real time. This felt like rapidly turning the steering wheel.
  * When the P-paramter was turned up, in general it led to oscillations about the middle of the track. To reduce these, I turned up the D-coefficient. This induced the car to begin straightening out on its way back to the center line. 
  * Finally, I noticed that when travelling around curves, the car would need to turn wide before it would return to the center. I figured this was a form of systematic bias, so I turned up the I-coefficient.

Of note is that the relative values of each of the coefficients is quite different. There is no problem with this. But the reason it is likely like this is because of the effect of the time step, which affects both the value of the derivative and the integral.

A very basic PID loop was used to regulate the speed as well. A constant throttle tended to saturate at too high a value. However there is some friction incorporated into the simulator, which has an effect similar to that of the D-coefficient. Therefore, I set the D-coefficient to 0 and set the I-coefficient to a low value, so that it would gradually reach the set point.

The live parameters were controlled from `params/init.json`, which was then read into a json object using the library already imported for communication with the simulator. There are two hard-coded flags which can turn on reliance on the json file at build time. One of them controls the ability to use the json file to initialized the PID object and the other controls the ability to use it in a live manner. As submitted, both flags are configured so that the json file is not needed and the PID parameters are hard coded and static. However, the json file is submitted along with the repository for reference. Also included is a video of the car completing a circuit: `media/around_the_track.mp4`
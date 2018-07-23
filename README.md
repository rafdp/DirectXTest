# Light refraction in water modeling

This project is an attempt at modeling the way light refracts in water on borders of layers with different temperatures. 

## About the model
### Box scaling
Researching the net showed that real-like parameters for water IOR at different temperatures applied to a limited volume of water 
does not affect passing light in any observable way, so the model is highly compressed over the z axis (in height). 
### Refraction
The temperature gradient is always collinear to the z axis, and temperature changes linearly around where the two layers with different temperatures converge. 
This way, the ray of light is continually passing through boundaries between water with different IORs, and Snell's law can be applied.
### Water and light modeling
The water model consists of a changeable number of semi-transparent particles, rendered through Direct3D. 
The ray of light is modeled using a starting point and directional vector, and the raytracing algorithm applies color to the affected particles of water. 

## Program structure
The most problematic part was building a user-friendly class-based interface to work with Direct3D.
The interface consists of a main processor for Direct3D, texture, shader and constant buffer managers and other support features.

Raytracing, refraction and the particle system are part of the model.

Error management based on self-developed cascade exception system (see [ScriptCompiler](https://github.com/rafdp/ScriptCompiler)). 


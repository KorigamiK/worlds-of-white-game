# Worlds of White

This is a video game prototype that I've long since given up on, but I'm
publishing it since it had some cool ideas.

It conceived various concepts at different points in the history, but its most
prevalent rendition was to be a simple puzzle platformer with a hand-drawn-like
rendering technique. The below video demonstrates what it most recently looked
like:

https://github.com/kmdreko/worlds-of-white-game/raw/refs/heads/master/demo/demo.mp4

There is more history of the development way back in my Twitter posts:
[@kmdreko_](https://x.com/kmdreko_)

## Building

The Visual Studio solution should be functional, but at the time this repo was
published I was unable to get this code to run. Though only due to linker
errors with third-party libraries that could probably be resolved with a bit of
gusto.

## About the Code

At one point I was an avid C++ developer and would've probably taken pride in
this code. Unfortunately a lot of that knowledge has been pushed out by Rust and
Typescript such that I can not really evaluate it.

Since the rendering technique involved some specialized mesh knowledge, there
are Python scripts that were used to extract the relevant data from Blender.

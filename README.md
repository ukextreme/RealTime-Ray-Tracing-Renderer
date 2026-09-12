# Real-Time Ray Tracing Renderer

A path-traced renderer in C++20.

This repository holds the **minimal rendering core**: analytic ray-sphere intersection, cosine-free
hemisphere sampling, multi-bounce indirect light, and gamma-corrected PPM output.

```bash
g++ -std=c++20 -O2 main.cpp -o rt
./rt > out.ppm
```

Default scene is two spheres over a large floor sphere with a single emissive light, rendered at
320x180 with 32 samples/pixel and 5 bounces.

## Status

Core only. The BVH builder, multi-threaded tile scheduler, and the benchmark harness are not part
of this repository.

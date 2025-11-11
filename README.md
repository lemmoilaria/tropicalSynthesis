tropicaleadd~ is a Max/MSP external written in C that implements tropical synthesis — a synthesis approach based on the mathematical principles of tropical algebra.
The object allows up to 100 oscillators to interact through spectral distortion, producing evolving harmonic structures, microtonal clusters, and unstable timbral behaviours.
For a detailed discussion of the mathematical background, see: Giorgio Sancristoforo https://www.giorgiosancristoforo.net/Tropical/paper.pdf

Description

Tropical additive synthesis replaces the mathematics behind classical additive synthesis with the logic of tropical mathematics, where:
a + b = min{a, b}
a * b = a + b

In tropical synthesis, summation between signals is replaced by the minimum between them, while multiplication becomes addition. This generates complex and non-linear harmonic behaviours even with a small number of oscillators.

The synthesis formula becomes:
trop(x[n]) = min{ a1 + cos(ω1n + φ1), a2 + cos(ω2n + φ2), ..., ap + cos(ωpn + φp) }
where:
a(i) = amplitude
ω(i) = angular frequency of the sine wave
φ(i) = phase

Parameters:

Spectral Distortion
The spectral distortion parameter defines how oscillators relate to the fundamental frequency.
Positive values: frequencies follow the harmonic spectrum (or inharmonic if the partial entered is float).
Negative values (-1 to -10): frequencies are randomised around the fundamental, generating beatings and complex detunings.
Each negative value defines a proportional random range:
-1 → ±2, -5 → ±10, -7 → ±14, etc.
These offsets are then added to the base frequency.

Amplitude
In tropical synthesis, amplitude is added to the signal, not multiplied.
If amplitude > 0, all oscillators share the same amplitude.
If amplitude < 0, each oscillator has a random amplitude between 0 and 1.
This design encourages dynamic balance and subtle irregularities between oscillators.

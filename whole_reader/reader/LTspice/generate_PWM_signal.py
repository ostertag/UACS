import math

frq = 64e3  # PWM frequency (64 kHz)

Voff = 0.0  # Low voltage level (0V)
Von = 3.3  # High voltage level (3.3V)

Trise = 0.1e-6  # Rise time (100 ns)
Tfall = 0.1e-6  # Fall time (100 ns)

audio_sample_rate = 16e3  # Audio sample rate (16 kHz)
audio_sample_period = 1 / audio_sample_rate  # Audio sample period (62.5 us)

# deriver parameters

pwm_period = 1 / frq  # PWM period (15.625 us)
pwm_periods_per_sample = int(
    frq / audio_sample_rate
)  # Number of PWM periods per audio sample

# The minimum amplitude is determined by the rise time and PWM period
min_amplitude = (
    1.5 * Trise / pwm_period
)  # Minimum amplitude of the audio signal (normalized between 0 and 1)
max_amplitude = (
    1 - 1.5 * Tfall / pwm_period
)  # Maximum amplitude of the audio signal (normalized between 0 and 1)

# Generate audio_samples as a 1kHz sine wave normalized between 0 and 1
audio_frq = 1000  # 1 kHz
sample_time = 10e-3  # Sample time (10 ms)
periods = sample_time * audio_frq  # Number of periods to generate

audio_sample_times = [
    i * audio_sample_period
    for i in range(int(periods * (1 / audio_frq) / audio_sample_period))
]

audio_samples = [
    (
        t,
        0.5
        * (math.sin(2 * math.pi * audio_frq * t) + 1)
        * (max_amplitude - min_amplitude)
        + min_amplitude,
    )
    for t in audio_sample_times
]

pwl_lines = []
prev_time = -1  # Initialize previous time to be less than 0


def add(time, voltage):
    global pwl_lines, prev_time
    if time <= prev_time:
        raise ValueError("Time must be increasing")
    prev_time = time
    pwl_lines.append(f"{time:.8e} {voltage}")


time = 0
add(time, Voff)  # Initial time and voltage

for time, sample_value in audio_samples:
    if sample_value < min_amplitude:
        sample_value = min_amplitude
    elif sample_value > max_amplitude:
        sample_value = max_amplitude
    for _ in range(pwm_periods_per_sample):
        time += Trise  # Add rise time
        add(time, Von)  # Add high voltage level
        time += (sample_value * pwm_period) - Trise  # Add time for the sample
        add(time, Von)  # Add high voltage level
        time += Tfall  # Add fall time
        add(time, Voff)  # Add low voltage level
        time += (
            1 - sample_value
        ) * pwm_period - Tfall  # Add time for the low voltage level
        add(time, Voff)  # Add low voltage level

# Write PWM to PWL file
with open("pwm_signal.pwl", "w") as f:
    f.write("\n".join(pwl_lines))

prev_time = -1  # Reset time for audio signal
pwl_lines = []  # Reset pwl_lines for audio signal

for time, sample_value in audio_samples:
    if sample_value < min_amplitude:
        sample_value = min_amplitude
    elif sample_value > max_amplitude:
        sample_value = max_amplitude
    add(time, sample_value)

# Write audio signal to PWL file
with open("audio_signal.pwl", "w") as f:
    f.write("\n".join(pwl_lines))

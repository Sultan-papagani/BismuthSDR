from math import floor, log2, gcd, sin, cos

# this DSP code is for a "rational resampler" using "polyphase resampler" and a "power decimator"
# pre-decimation is "power_decimator" (fir filter + take every n'th sample)
# "power_decimator" is literally a decimating fir filter.. but only works in powers of two (2,4,6,8..).

# YOUR INPUT VARIABLES (default variables are just a example; here its 200khz to 44100hz conversion)
double_inSampleRate =  200000
double_outSampleRate = 44100
# DONT TOUCH THE REST


mode = "null"
DB_M_PI = 3.14159265358979323846

def hzToRads(freq, samplerate):
    return 2.0 * DB_M_PI * (freq / samplerate)

def cosine(n, N, coefs, coefCount):
    win = 0
    sign = 1
    for i in range(coefCount):
        win += sign * coefs[i] * cos(i * 2.0 * DB_M_PI * n / N)
        sign = -sign
    return win

def nuttal(n, N):
    coefs = [0.355768, 0.487396, 0.144232, 0.012604]
    return cosine(n, N, coefs, len(coefs))

def sinc(x):
    if x == 0.0:
        return 1.0
    else:
        return sin(x) / x

def windowed_sinc(tap_count, double_omega, Window, double_norm = 1.0):
    # Allocate taps
    taps = [] # size: tap_count

    # Generate using window
    double_half = tap_count / 2.0
    double_corr = double_norm * double_omega / DB_M_PI;

    for i in range(int(tap_count)):
        t = i - double_half + 0.5
        # Type is float and window == "NUTTAL"
        if (True):
            taps.append(sinc(t * double_omega) * nuttal(t - double_half, tap_count) * double_corr)
        # Type is complex and window could be something else
        else:
            pass

    return taps

def low_pass_windowed_sinc(cutoff, transWidth, samplerate, oddTapCount = False):
    #estimate tap count
    tap_count = 3.8 * samplerate / transWidth
    if oddTapCount and not (tap_count % 2):
        count += 1
    return windowed_sinc(tap_count, hzToRads(cutoff, samplerate), "NUTTAL", 1.0)
    pass

#Calculate highest power-of-two decimation for the power decimator 
int_predecPower = int(min(floor(log2(double_inSampleRate / double_outSampleRate)), 8192))
if int_predecPower < 0:
    int_predecRatio = 8192
else:
    int_predecRatio = int(min(1 << int_predecPower, 8192))
double_intSamplerate = double_inSampleRate

# Configure the DDC
bool_useDecim = (double_inSampleRate > double_outSampleRate and int_predecPower > 0)
if bool_useDecim:
    double_intSamplerate = double_inSampleRate / int_predecRatio;
    print(f"pre-decimation Power: {int_predecPower}\npre-decimation Ratio: {int_predecRatio}\n")
    # set "power-of-two decimation" ratio to int_predecRatio

# Calculate interpolation and decimation for polyphase resampler
int_IntSR = round(double_intSamplerate)
int_OutSR = round(double_outSampleRate)
int_gcd = gcd(int_IntSR, int_OutSR)
int_interp = int_OutSR / int_gcd
int_decim = int_IntSR / int_gcd

# Check for excessive error
double_actualOutSR = int_IntSR * int_interp / int_decim
double_error = abs((double_actualOutSR - double_outSampleRate) / double_outSampleRate) * 100.0
print(f"actual Out Samplerate: {double_actualOutSR}\nerror: {double_error}%\n")
if double_error > 0.01:
    print(f"Warning: resampling error is over {double_error}%\n")

# If the power decimator already did all the work, don't use the resampler
if int_interp == int_decim:
    if bool_useDecim:
        mode = "decim only"
    else:
        mode = "none"
    print(f"in SampleRate: {double_inSampleRate}\nout SampleRate: {double_outSampleRate}\nafter pre-decimation Samplerate: {double_intSamplerate}\nmode: {mode}\ngcd: {int_gcd}\ninterp: {int_interp}\ndecim: {int_decim}")
    exit() # return.

# Configure the polyphase resampler
double_tapSamplerate = double_intSamplerate * int_interp
double_tapBandwidth = min(double_inSampleRate, double_outSampleRate) / 2.0;
double_tapTransWidth = double_tapBandwidth * 0.1;

# generate taps
rtaps = low_pass_windowed_sinc(double_tapBandwidth, double_tapTransWidth, double_tapSamplerate)
for i in range(len(rtaps)):
    rtaps[i] *= int_interp

print(f"polyphase rtaps length: {len(rtaps)}\npolyphase taps: {rtaps[1]},{rtaps[2]},{rtaps[3]},{rtaps[4]}...\n")

if bool_useDecim:
    mode = "decim and resamp"
else:
    mode = "resamp only"

print(f"tap Samplerate: {double_tapSamplerate}\ntap Bandwidth: {double_tapBandwidth}\ntap Trans Width: {double_tapTransWidth}\n")
print(f"in SampleRate: {double_inSampleRate}\nout SampleRate: {double_outSampleRate}\nafter pre-decimation Samplerate: {double_intSamplerate}\nmode: {mode}\ngcd: {int_gcd}\ninterp: {int_interp}\ndecim: {int_decim}")



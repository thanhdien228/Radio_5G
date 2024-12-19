import numpy as np  # use fft
import matplotlib.pyplot as plt
import argparse  # support parse command


def read_fft_data(file_path):
    with open(file_path, "r") as file:
        lines = file.read().split()
    complex_data = np.array(lines, dtype=np.complex128)
    return complex_data


def process_data(complex_data, fs):
    fft_result = np.fft.fft(complex_data)
    fft_freqs = np.fft.fftfreq(fft_result.size, d=1/fs)
    return fft_result, fft_freqs


def plot_frequency_domain(fft_result, fft_freqs, name_pic):
    plt.figure()
    N = fft_result.size
    amp = np.abs(fft_result[:N // 2]) * 2 / N
    freq = fft_freqs[:N // 2]
    ax = plt.gca()
    ax.set_ylim([0, np.max(amp)*1.2])
    ax.set_xlim([0, np.max(freq)])
    plt.plot(freq[np.argmax(amp)], np.max(amp), 'rx')  # show max
    plt.plot(freq, amp)
    plt.title("Frequency Domain")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Amplitude")
    plt.savefig(name_pic)
    plt.close()


def parse_command(parser):
    parser.add_argument("--output", help="an output file")
    parser.add_argument("--input", help="an input data")
    parser.add_argument("--fs", type=float, help="frequency sample")
    args = parser.parse_args()
    return args


parser = argparse.ArgumentParser()
args = parse_command(parser)
complexData = read_fft_data(args.input)
fft_result, fft_freqs = process_data(complexData, args.fs)
plot_frequency_domain(fft_result, fft_freqs, args.output)

import numpy as np  # use fft
import matplotlib.pyplot as plt
import argparse  # support parse command

def read_data_noise(file_path_noise):
    with open(file_path_noise, "r") as file:
        lines = file.read().split()
    noise_data_number = np.array(lines, dtype=np.float64)
    return noise_data_number

def read_data_filtered(file_path_filtered):
    with open(file_path_filtered, "r") as file:
        lines = file.read().split()
    filtered_data_number = np.array(lines, dtype=np.float64)
    return filtered_data_number

def draw_bin_signal(binString, data_number, fs, fc):
    words = list(binString)  # Splits by whitespace
    binSignal = []
    for char in words:
        binSignal.append(int(char))
    bin_array = np.array(binSignal)
    samples = int(fs/fc) # Samples in one bit duration (250s)
    float_num = int(np.floor(np.size(data_number)/samples)) # make the number from 0-1
    sig_binary = np.zeros(int(float_num*samples)) # draw
    lst  = np.where(bin_array == 1)
    for i in lst[0]:
        temp = int(i*samples)
        sig_binary[temp:temp + samples] = 1
    return sig_binary , samples

def process_data_noise(noise_data_number, fs):
    noise_wavepoint = noise_data_number
    timeDomain = np.linspace(0,(1/fs)* np.size(noise_data_number), np.size(noise_data_number))
    return noise_wavepoint, timeDomain

def process_data_filtered(filtered_data_number, fs):
    filtered_wavepoint = filtered_data_number
    timeDomain = np.linspace(0,(1/fs)* np.size(filtered_data_number), np.size(filtered_data_number))
    return filtered_wavepoint, timeDomain


def plot_domain(timeDomain, noise_ampDomain, filtered_ampDomain, binDomain, name_pic):
    plt.figure()
    fig, axs = plt.subplots(3)
    plt.subplots_adjust(left=0.1, bottom=0.1, top=0.9, wspace=0.4,hspace=0.6)
    axs[0].plot(timeDomain, noise_ampDomain)
    axs[0].set_xlabel("Time (s)")
    axs[0].set_ylabel("Amplitude")
    axs[1].plot(timeDomain, filtered_ampDomain)
    axs[1].set_xlabel("Time (s)")
    axs[1].set_ylabel("Amplitude")
    axs[2].plot(timeDomain, binDomain)
    axs[2].set_xlabel("Time (s)")
    axs[2].set_ylabel("Bin signal")
    plt.show()
    plt.savefig(name_pic)
    plt.close()


def parse_command(parser):
    parser.add_argument("--output", help="an output file")
    parser.add_argument("--inputNoise", help="an input data")
    parser.add_argument("--inputFiltered", help="an input data")
    parser.add_argument("--fs", type=int, help="frequency sample")
    parser.add_argument("--bin",type=str, help="binary sample")
    parser.add_argument("--fc",type = int, help = "carrier frequency" )
    args = parser.parse_args()
    return args


parser = argparse.ArgumentParser()
args = parse_command(parser)
noiseData = read_data_noise(args.inputNoise)
filteredData = read_data_filtered(args.inputFiltered)
binSignal, sample = draw_bin_signal(args.bin, filteredData, args.fs, args.fc)
noise_wavepoint, timeDomain = process_data_noise(noiseData, args.fs)
filtered_wavepoint, timeDomain = process_data_filtered(filteredData,args.fs)
plot_domain(timeDomain, noise_wavepoint, filtered_wavepoint , binSignal, args.output)

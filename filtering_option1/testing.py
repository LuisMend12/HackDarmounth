import soundfile as sf
import os

folder_path = r"C:\Users\Luis Mendez\source\repos\test\HackDarmounth\filtering_option1\data"

quant_step = 1  # Aggressive rounding
tolerance =  1   # Aggressive filtering
k_samples = 200

wav_files = [f for f in os.listdir(folder_path) if f.lower().endswith('.wav')]

print(f"Found {len(wav_files)} wav files.")

count = 0

# Open output file
with open('compressed_data_light.txt', 'w') as outfile:

    for filename in wav_files:
        full_path = os.path.join(folder_path, filename)

        data, samplerate = sf.read(full_path, dtype='int16')

        n_samples = min(k_samples, len(data))

        l_value = int(data[0] / quant_step) * quant_step

        for i in range(n_samples):
            current_value = int(data[i] / quant_step) * quant_step

            if abs(current_value - l_value) > tolerance:
                outfile.write(f"{current_value}\n")  # write to text file
                l_value = current_value

                count += 1

print(f"count {count}")

from pydub import AudioSegment
import os
import noisereduce as nr
import numpy as np

# Input and output folder paths
input_folder = r"C:\Users\Luis Mendez\source\repos\test\HackDarmounth\filtering_option1\data"
output_folder = r"C:\Users\Luis Mendez\source\repos\test\HackDarmounth\filtering_option1\2_filtered_data"

# Ensure output folder exists
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# Function to filter static noise, amplify beep, and compress
def process_audio(input_path, output_path):
    # Load the audio file
    audio = AudioSegment.from_wav(input_path)

    # Convert to numpy array for processing
    samples = np.array(audio.get_array_of_samples())

    # Reduce static noise
    reduced_noise_samples = nr.reduce_noise(y=samples, sr=audio.frame_rate)

    # Convert back to audio
    reduced_noise_audio = AudioSegment(
        reduced_noise_samples.astype(np.int16).tobytes(),
        frame_rate=audio.frame_rate,
        sample_width=audio.sample_width,
        channels=audio.channels
    )

    # Amplify the beeping sound
    amplified_audio = reduced_noise_audio + 20

    # Set output path to .mp3
    output_file_path = os.path.join(output_folder, os.path.splitext(os.path.basename(output_path))[0] + ".mp3")

    # Export as MP3 with lossy compression (control bitrate)
    amplified_audio.export(output_file_path, format="mp3", bitrate="128k")  # 128 kbps, you can lower to 64k if needed

# Process all WAV files
for filename in os.listdir(input_folder):
    if filename.endswith(".wav"):
        input_path = os.path.join(input_folder, filename)
        output_path = os.path.join(output_folder, filename)
        process_audio(input_path, output_path)
        print(f"Processed {filename}")

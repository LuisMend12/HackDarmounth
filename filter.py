import os
import noisereduce as nr
from pydub import AudioSegment
import numpy as np

# Input and output folder paths
input_folder = r"C:\Users\Luis Mendez\source\repos\test\HackDarmounth\data"
output_folder = r"C:\Users\Luis Mendez\source\repos\test\HackDarmounth\filtered_data"

# Ensure output folder exists
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# Function to filter static noise and amplify beeping sound
def process_audio(input_path, output_path):
    # Load the audio file
    audio = AudioSegment.from_wav(input_path)

    # Convert to numpy array for processing
    samples = np.array(audio.get_array_of_samples())

    # Reduce static noise using spectral subtraction (noisereduce)
    reduced_noise_samples = nr.reduce_noise(y=samples, sr=audio.frame_rate)

    # Convert the reduced noise samples back to audio
    reduced_noise_audio = AudioSegment(
        reduced_noise_samples.astype(np.int16).tobytes(), 
        frame_rate=audio.frame_rate, 
        sample_width=audio.sample_width, 
        channels=audio.channels
    )

    # Amplify the beeping sound (increase the volume)
    amplified_audio = reduced_noise_audio + 20  # Amplify by 10 dB, adjust as needed

    # Export the processed audio to the output folder
    output_file_path = os.path.join(output_folder, os.path.basename(output_path))
    amplified_audio.export(output_file_path, format="wav")

# Process all WAV files in the input folder
for filename in os.listdir(input_folder):
    if filename.endswith(".wav"):
        input_path = os.path.join(input_folder, filename)
        output_path = os.path.join(output_folder, filename)
        process_audio(input_path, output_path)
        print(f"Processed {filename}")

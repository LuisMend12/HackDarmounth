import os
import time
import numpy as np
import csv
from pydub import AudioSegment
import noisereduce as nr

# Input and output folder paths
input_folder = r"C:\Users\Luis Mendez\source\repos\test\HackDarmounth\filtering_option1\data"
output_folder = r"C:\Users\Luis Mendez\source\repos\test\HackDarmounth\filtering_option1\filtered_data"

# Ensure output folder exists
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# Path for storing processed files record
processed_files_path = "processed_files.txt"

# Function to compute SNR (Signal-to-Noise Ratio)
def compute_snr(audio_samples, reduced_audio_samples):
    signal_power = np.mean(audio_samples**2)
    noise_power = np.mean((audio_samples - reduced_audio_samples)**2)

    # Avoid division by zero or log10 of zero by checking signal and noise power
    if signal_power == 0:
        return 0  # SNR is zero if there's no signal
    if noise_power == 0:
        return np.inf  # Infinite SNR if there's no noise

    snr = 10 * np.log10(signal_power / noise_power)
    return snr

# Function to compute RMSE (Root Mean Square Error)
def compute_rmse(original_samples, processed_samples):
    return np.sqrt(np.mean((original_samples - processed_samples)**2))

# Function to process and test the audio
def process_and_test_audio(input_path, output_path):
    start_time = time.time()  # Start timing

    # Load the audio file
    audio = AudioSegment.from_wav(input_path)
    original_samples = np.array(audio.get_array_of_samples())

    # Perform noise reduction
    reduced_noise_samples = nr.reduce_noise(y=original_samples, sr=audio.frame_rate)

    # Check if noise reduction left us with an array of zeros (or close to zero)
    if np.allclose(reduced_noise_samples, 0):
        print(f"Warning: Noise reduction produced a signal of zeros for {os.path.basename(input_path)}.")
        reduced_noise_samples = original_samples  # Fallback to original audio

    # Calculate SNR before and after noise reduction
    snr_before = compute_snr(original_samples, original_samples)
    snr_after = compute_snr(original_samples, reduced_noise_samples)

    # Compute RMSE between original and filtered audio
    rmse = compute_rmse(original_samples, reduced_noise_samples)

    # Convert to AudioSegment for further processing
    reduced_noise_audio = AudioSegment(
        reduced_noise_samples.astype(np.int16).tobytes(),
        frame_rate=audio.frame_rate,
        sample_width=audio.sample_width,
        channels=audio.channels
    )

    # Amplify the beep
    amplified_audio = reduced_noise_audio + 20

    # Set output path to .mp3
    output_file_path = os.path.join(output_folder, os.path.splitext(os.path.basename(output_path))[0] + ".mp3")

    # Export as MP3
    amplified_audio.export(output_file_path, format="mp3", bitrate="128k")

    end_time = time.time()  # End timing
    elapsed_time = end_time - start_time  # Time taken in seconds

    return {
        "filename": os.path.basename(input_path),
        "processing_time": elapsed_time,
        "snr_before": snr_before,
        "snr_after": snr_after,
        "rmse": rmse
    }

# Function to calculate averages and save data
def calculate_averages_and_save(data, output_csv_file):
    # Calculate averages
    avg_processing_time = np.mean([entry["processing_time"] for entry in data])
    avg_snr_before = np.mean([entry["snr_before"] for entry in data])
    avg_snr_after = np.mean([entry["snr_after"] for entry in data])
    avg_rmse = np.mean([entry["rmse"] for entry in data])

    # Display the averages
    print(f"Average Processing Time: {avg_processing_time:.2f} seconds")
    print(f"Average SNR before noise reduction: {avg_snr_before:.2f} dB")
    print(f"Average SNR after noise reduction: {avg_snr_after:.2f} dB")
    print(f"Average RMSE between original and filtered: {avg_rmse:.5f}")

    # Save the data to a CSV file
    with open(output_csv_file, mode="w", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=data[0].keys())
        writer.writeheader()
        writer.writerows(data)

# Function to load already processed files from a text file
def load_processed_files():
    if os.path.exists(processed_files_path):
        with open(processed_files_path, 'r') as file:
            return set(file.read().splitlines())
    return set()

# Function to save the list of processed files to a text file
def save_processed_files(processed_files):
    with open(processed_files_path, 'w') as file:
        file.write("\n".join(processed_files))

# Main function to process all files
def process_all_files():
    # Load the list of already processed files
    processed_files = load_processed_files()

    # Get all WAV files in the input folder
    all_files = [f for f in os.listdir(input_folder) if f.endswith(".wav")]
    
    # Get the unprocessed files
    unprocessed_files = [f for f in all_files if f not in processed_files]
    
    if not unprocessed_files:
        print("All files have already been processed.")
        return

    # List to store results
    all_data = []

    # Process each file
    for filename in unprocessed_files:
        input_path = os.path.join(input_folder, filename)
        output_path = os.path.join(output_folder, filename)
        result = process_and_test_audio(input_path, output_path)
        all_data.append(result)

        # Mark the file as processed
        processed_files.add(filename)

    # Calculate averages and save data to CSV
    output_csv_file = "audio_processing_results.csv"
    calculate_averages_and_save(all_data, output_csv_file)

    # Save the updated list of processed files
    save_processed_files(processed_files)

# Process all files
process_all_files()

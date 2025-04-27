🧠 BrainFlux
BrainFlux is a cutting-edge real-time neural data visualization tool built with C++ and ImGui, designed to help researchers and engineers intuitively explore and analyze complex brain activity data.
Originally developed for HackDarmouth, BrainFlux focuses on rendering high-speed neuronal spike and brainwave data with responsive, interactive controls, making insights easier to uncover.

🚀 Inspiration
Neural data — especially from interfaces like Neuralink’s N1 implant — can be overwhelming and complex.
We wanted a powerful, lightweight visualizer that makes sense of this data in real time without sacrificing speed, flexibility, or clarity.

🧩 What It Does
Visualizes real-time neural spike data and synaptic patterns

Provides smooth, interactive graphs using ImGui

Handles large-scale brainwave datasets with minimal latency

Offers an intuitive interface designed for researchers and engineers

🛠️ How We Built It
Visualization: Developed a C++ application using ImGui for high-performance, real-time data rendering.

Data Preprocessing:

Noise Filtering: Used Python to remove static noise and amplify meaningful signals from raw brainwave audio data.

Lossy Compression: Applied lossy compression to reduce dataset size for faster processing and lightweight storage.

<details>
<summary><strong>Noise Filtering (Python)</strong></summary>

<pre><code>import os
import noisereduce as nr
from pydub import AudioSegment
import numpy as np

input_folder = r"C:\path\to\data"
output_folder = r"C:\path\to\filtered_data"

def process_audio(input_path, output_path):
    audio = AudioSegment.from_wav(input_path)
    samples = np.array(audio.get_array_of_samples())
    reduced_noise_samples = nr.reduce_noise(y=samples, sr=audio.frame_rate)
    reduced_noise_audio = AudioSegment(
        reduced_noise_samples.astype(np.int16).tobytes(),
        frame_rate=audio.frame_rate,
        sample_width=audio.sample_width,
        channels=audio.channels
    )
    amplified_audio = reduced_noise_audio + 20
    output_file_path = os.path.join(output_folder, os.path.basename(output_path))
    amplified_audio.export(output_file_path, format="wav")

for filename in os.listdir(input_folder):
    if filename.endswith(".wav"):
        process_audio(os.path.join(input_folder, filename), os.path.join(output_folder, filename))
</code></pre>

</details>

---

<details>
<summary><strong>Lossy Compression (Python)</strong></summary>

<pre><code>from pydub import AudioSegment
import os
import noisereduce as nr
import numpy as np

input_folder = r"C:\path\to\data"
output_folder = r"C:\path\to\2_filtered_data"

def process_audio(input_path, output_path):
    audio = AudioSegment.from_wav(input_path)
    samples = np.array(audio.get_array_of_samples())
    reduced_noise_samples = nr.reduce_noise(y=samples, sr=audio.frame_rate)
    reduced_noise_audio = AudioSegment(
        reduced_noise_samples.astype(np.int16).tobytes(),
        frame_rate=audio.frame_rate,
        sample_width=audio.sample_width,
        channels=audio.channels
    )
    amplified_audio = reduced_noise_audio + 20
    output_file_path = os.path.join(output_folder, os.path.splitext(os.path.basename(output_path))[0] + ".mp3")
    amplified_audio.export(output_file_path, format="mp3", bitrate="128k")

for filename in os.listdir(input_folder):
    if filename.endswith(".wav"):
        process_audio(os.path.join(input_folder, filename), os.path.join(output_folder, filename))
</code></pre>

</details>
⚡ Challenges We Ran Into
Managing real-time rendering while keeping performance smooth

Cleaning noisy spike recordings without losing important brain signals

Designing a UI that is both minimal and powerful for scientific use

🏆 Accomplishments
Built a complete real-time brainwave visualizer from scratch

Successfully reduced noise and compressed large datasets efficiently

Designed a responsive ImGui-based user interface

📚 What We Learned
Advanced noise reduction and audio processing with Python

Real-time rendering techniques with C++ and ImGui

Efficiently handling and visualizing complex biological datasets

🔮 What's Next for BrainFlux
Integrating machine learning for brain pattern detection

Adding support for live neural data streaming

Exporting analytical reports and data summaries


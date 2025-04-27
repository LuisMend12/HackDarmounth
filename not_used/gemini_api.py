# Import libraries
from google import generativeai as genai
from dotenv import load_dotenv
import os

# Load environment variables from .env file
load_dotenv()

# Get the API key from environment variables
api_key = os.getenv("GOOGLE_API_KEY")

# Configure the genai client
genai.configure(api_key=api_key)

# Create the Generative Model
model = genai.GenerativeModel(model_name="gemini-2.0-flash")

# Send a prompt to the model
response = model.generate_content("Explain how AI works in a few words")

# Print the response text
print(response.text)

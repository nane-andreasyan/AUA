import random

size = 100 * 1024 * 1024
chars = b"ACGT"
chunk_size = 1024 * 1024  

with open("dna.txt", "wb") as f:
    for _ in range(size // chunk_size):
        chunk = bytes(random.choice(chars) for _ in range(chunk_size))
        f.write(chunk)
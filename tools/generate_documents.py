from pathlib import Path


documents_dir = Path("generated_documents")
documents_dir.mkdir(exist_ok=True)
num_documents = 10000

for i in range(num_documents):
    file_path = documents_dir / f"document_{i+1}.txt"
    content = f"Document {i+1} contains information about distributed systems, search engines, programming, and computers"
    file_path.write_text(content)

print(f"Generated {num_documents} documents.")
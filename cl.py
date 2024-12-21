import os

def remove_newlines_from_file(file_path):
    with open(file_path, 'r') as file:
        content = file.read()
    
    content_without_newlines = content.replace('\n', '')
    
    with open(file_path, 'w') as file:
        file.write(content_without_newlines)

def process_files_in_directory(directory_path):
    for filename in os.listdir(directory_path):
        file_path = os.path.join(directory_path, filename)
        
        if os.path.isfile(file_path) and filename.endswith('.txt'):
            remove_newlines_from_file(file_path)
            print(f"Newlines removed from {filename}")

if __name__ == "__main__":
    neg_clean_directory = "neg-clean"
    
    if os.path.exists(neg_clean_directory) and os.path.isdir(neg_clean_directory):
        process_files_in_directory(neg_clean_directory)
        print("Newlines removed from all text files in 'neg-clean' directory.")
    else:
        print("Error: 'neg-clean' directory not found.")

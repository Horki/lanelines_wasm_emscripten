import os, shutil

print("Post Build Script")
os.makedirs("web", exist_ok=True)

shutil.copy('img/img.jpg', 'web/')
shutil.copy('build/LaneLines.js', 'web/')
shutil.copy('build/LaneLines.wasm', 'web/')
shutil.copy('static/index.js', 'web/')
shutil.copy('static/index.html', 'web/')
shutil.copy('static/style.css', 'web/')

print("Ok!")
print("cd web && python -m http.server")
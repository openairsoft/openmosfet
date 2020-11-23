Import("env")

print("building ui")
env.Execute("cd src/ui && npm run build-dev")
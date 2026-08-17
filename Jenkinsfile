pipeline {
    agent any
    stages {
        stage('Generate Model') {
            steps { sh 'python3 tools/generate_model.py' }
        }
        stage('Configure') {
            steps { sh 'cmake -S . -B build -DCMAKE_BUILD_TYPE=Release' }
        }
        stage('Build') {
            steps { sh 'cmake --build build -j2' }
        }
        stage('Test') {
            steps { sh 'ctest --test-dir build --output-on-failure' }
        }
    }
}

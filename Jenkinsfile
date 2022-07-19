pipeline {
    agent {
        dockerfile {
            filename 'Dockerfile'
            dir 'docker'
        }
    }
    stages {
        stage('Test') {
            steps {
                sh 'echo "Building for target ${target}..."'
                sh 'ls -al'
            }
        }
    }
}

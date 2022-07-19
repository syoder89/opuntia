pipeline {
    agent {
        dockerfile {
            filename 'Dockerfile'
            dir 'docker'
            label 'opuntia'
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

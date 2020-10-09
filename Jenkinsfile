@Library("islog-helper") _

pipeline {
    agent none

    options {
        gitLabConnection('Gitlab Pontos')
        disableConcurrentBuilds()
        lock label: 'CONAN_CONFIGURATION_LOCK', quantity: 1
    }

    environment {
        PACKAGE_NAME = "LogicalAccessNFC/2.3.0@islog/${BRANCH_NAME}"
        LINUX_DOCKER_IMAGE_NAME = 'docker-registry.islog.com:5000/conan-recipes-support:latest'
        // This is needed because MSBuild fails spuriously quiet often
        // on the build machine.
        MSBUILDDISABLENODEREUSE = 1

        CONAN_REVISIONS_ENABLED = 1
    }
    parameters {
        booleanParam(name: 'BUILD_DEBUG',
                defaultValue: true,
                description: 'Perform DEBUG build')
        booleanParam(name: 'BUILD_RELEASE',
                defaultValue: true,
                description: 'Perform RELEASE build')
        booleanParam(name: 'BUILD_WINDOWS',
                defaultValue: true,
                description: 'Perform Windows build')
        booleanParam(name: 'BUILD_LINUX',
                defaultValue: true,
                description: 'Perform Linux build')
    }

    stages {
        stage('Minimal Feature Build') {
            steps {
                script {
                    lst = []
                    if (params.BUILD_WINDOWS) {
                        if (params.BUILD_DEBUG) {
                            lst += 'lla/x64_msvc_debug_min'
                            lst += 'lla/x86_msvc_debug_min'
                        }
                        if (params.BUILD_RELEASE) {
                            lst += 'lla/x64_msvc_release_min'
                            lst += 'lla/x86_msvc_release_min'
                        }
                    }
                    if (params.BUILD_LINUX) {
                        if (params.BUILD_DEBUG) {
                            lst += 'lla/x64_gcc6_debug_min'
                        }
                        if (params.BUILD_RELEASE) {
                            lst += 'lla/x64_gcc6_release_min'
                        }
                    }
                    lla.startJobForProfiles(lst)
                }
            }
        }

        stage('Complete Feature Build') {
            steps {
                script {
                    lst = []
                    if (params.BUILD_WINDOWS) {
                        if (params.BUILD_DEBUG) {
                            lst += 'lla/x64_msvc_debug_full'
                            lst += 'lla/x86_msvc_debug_full'
                        }
                        if (params.BUILD_RELEASE) {
                            lst += 'lla/x64_msvc_release_full'
                            lst += 'lla/x86_msvc_release_full'
                        }
                    }
                    if (params.BUILD_LINUX) {
                        if (params.BUILD_DEBUG) {
                            lst += 'lla/x64_gcc6_debug_full'
                        }
                        if (params.BUILD_RELEASE) {
                            lst += 'lla/x64_gcc6_release_full'
                        }
                    }
                    lla.startJobForProfiles(lst)
                }
            }
        }
    }

    post {
        changed {
            script {
                if (currentBuild.currentResult == 'FAILURE' || currentBuild.currentResult == 'SUCCESS') {
                    // Other values: SUCCESS, UNSTABLE
                    // Send an email only if the build status has changed from green/unstable to red
                    emailext subject: '$DEFAULT_SUBJECT',
                            body: '$DEFAULT_CONTENT',
                            recipientProviders: [
                                    [$class: 'CulpritsRecipientProvider'],
                                    [$class: 'DevelopersRecipientProvider'],
                                    [$class: 'RequesterRecipientProvider']
                            ],
                            replyTo: 'cis@islog.com',
                            to: 'reports@islog.com'
                }
            }
        }
    }
}

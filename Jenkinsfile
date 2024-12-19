pipeline {
    agent {
        node {
            label 'build'
        }
    }

    options {
        checkoutToSubdirectory('git')
        disableConcurrentBuilds()
        timeout(time: 10, unit: 'MINUTES')
    }

    stages {
        stage('Build') {
            steps {
                sh '''#!/usr/bin/env bash
                set -ex

                source /opt/rh/gcc-toolset-13/enable

                printenv | sort

                git --version
                gcc --version
                g++ --version
                make --version
                cmake --version
                autoconf --version
                automake --version

                INSTALL_DIR=${WORKSPACE}/rootfs/
                mkdir -pv ${INSTALL_DIR}

                pushd git
                ls -alhs

                # Build database library
                cd database
                autoreconf -fi
                ./configure --prefix=${INSTALL_DIR}
                make
                make install

                # Build unit test database
                cd tests
                autoreconf -fi
                ./configure --prefix=${INSTALL_DIR}
                make check
                cd ..

                #Build logging library
                cd ../logging
                autoreconf -fi
                ./configure --prefix=${INSTALL_DIR}
                make
                make install
                
                #Build unit test logging
                cd tests
                autoreconf -fi
                ./configure --prefix=${INSTALL_DIR}
                make check
                cd ..

                #Build client
                cd ../client
                autoreconf -fi
                ./configure --prefix=${INSTALL_DIR}
                make

                export LD_LIBRARY_PATH=${INSTALL_DIR}/lib:${LD_LIBRARY_PATH}

                #Build server
                cd ../server
                autoreconf -fi
                ./configure --prefix=${INSTALL_DIR}
                make

                #build unit test for some classes in server
                cd tests
                autoreconf -fi
                ./configure --prefix=${INSTALL_DIR}
                make check
                popd

                tar -czvf rootfs.tar.gz rootfs
                tar -tvf rootfs.tar.gz
                '''
            }
        }
    }

    post {
        success {
            archiveArtifacts(
                artifacts: 'rootfs.tar.gz',
                fingerprint: true,
                onlyIfSuccessful: true
            )
        }

        cleanup {
            cleanWs()
        }

        always {
            emailext(
                subject: '${DEFAULT_SUBJECT}',
                recipientProviders: [developers(), requestor()],
                body: '${DEFAULT_CONTENT}',
                to: 'canh.dinh@lab.internal'
            )
        }
    }
}


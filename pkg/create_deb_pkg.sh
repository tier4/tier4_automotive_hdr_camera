#!/bin/sh

# Exit immediately if any command fails
set -e

# Default values
RELEASE_MODE=false

# Parse arguments
while [ "$#" -gt 0 ]; do
    case "$1" in
        --release|-r)
            RELEASE_MODE=true
            shift
            ;;
        *)
            echo "Unknown option: $1" >&2
            echo "Usage: $0 [--release|-r]" >&2
            exit 1
            ;;
    esac
done

# Get the git short hash
GIT_HASH=$(git rev-parse --short HEAD)

# Extract the base version from debian/changelog
CURRENT_VERSION=$(dpkg-parsechangelog -S Version)
BASE_VERSION=${CURRENT_VERSION%-*}

if [ "$RELEASE_MODE" = "true" ]; then
    # Generate a version string without git hash
    NEW_VERSION="${CURRENT_VERSION}"
    COMMIT_MSG="Automated release build"
else
    # Generate a dynamic version string (e.g., "2.1.0~8f6da0c-1")
    NEW_VERSION="${BASE_VERSION}~${GIT_HASH}"
    COMMIT_MSG="Automated build from git commit ${GIT_HASH}"
fi

echo "Updating changelog to version: ${NEW_VERSION}"
# Append a new changelog entry with the version
dch -b -v "${NEW_VERSION}" -m "${COMMIT_MSG}"

echo "Updating dkms.conf to version: ${NEW_VERSION}"
# Inject the dynamic version into dkms.conf and prerm
sed -i "s/@VERSION@/${NEW_VERSION}/g" dkms.conf debian/prerm

# Build the Debian package
dpkg-buildpackage -b -rfakeroot -us -uc

# Restore modified files to keep the git working tree clean
git checkout debian/changelog dkms.conf debian/prerm

echo "Build complete! Package generated with version ${NEW_VERSION}"
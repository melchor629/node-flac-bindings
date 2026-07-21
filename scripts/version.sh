#!/bin/bash

set -e

if [[ -z "$1" ]]; then
  echo "Usage: $0 <version>"
  exit 1
fi

VERSION="${1#v}"

if git tag -l | grep -q "^v$VERSION$"; then
  echo "Tag v$VERSION already exists. Please choose a different version."
  exit 1
fi

for package in packages/flac-bindings packages/flac-bindings-lib; do
  echo "Updating version in $package/package.json to $VERSION"
  cat <<< "$(jq --arg VERSION "$VERSION" '.version=$VERSION' "$package/package.json")" > "$package/package.json"
  if [[ "$package" == "packages/flac-bindings" ]]; then
    cat <<< "$(jq --arg VERSION "$VERSION" '.peerDependencies["@melchor629/flac-bindings-lib"]=$VERSION' "$package/package.json")" > "$package/package.json"
  fi
  git add "$package/package.json"
done

git commit -m "Version ${VERSION}"
git push

sleep 1 # thanks github actions
git tag "v$VERSION"
git push --tags

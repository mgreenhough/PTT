#!/bin/bash

echo "Staging all changes..."
git add .

read -p "Enter commit message: " commit_msg

if [ -z "$commit_msg" ]; then
  echo "Commit message cannot be empty. Aborting."
  exit 1
fi

git commit -m "$commit_msg"

echo "Pushing to remote..."
git push

echo "Done."

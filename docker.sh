docker build --platform=linux/amd64 -t smartnic-dev .

docker run -it \
    --mount type=bind,source=/run/host-services/ssh-auth.sock,target=/ssh-agent \
    --env SSH_AUTH_SOCK=/ssh-agent \
    --mount type=bind,source="$(pwd)",target=/smartnic-rewriter \
    smartnic-dev
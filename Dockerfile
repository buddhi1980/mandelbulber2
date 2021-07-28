# Base image
#
FROM archlinux:latest as base

LABEL org.label-schema.build-date=$BUILD_DATE \
      org.label-schema.name="Mandelbulber2" \
      org.label-schema.description="Fractal raytracer" \
      org.label-schema.url="https://github.com/buddhi1980/mandelbulber2" \
      org.label-schema.vendor="buddhi1980" \
      org.label-schema.version=$VERSION \
      org.label-schema.schema-version="1.0"

#
RUN pacman -Syu --noconfirm --noprogressbar && \
    pacman -Sy --noconfirm --noprogressbar \
    gsl \
    libpng12 \
    lzo \
    openmpi \
    qt5-base \
    qt5-multimedia \
    qt5-tools \
    ocl-icd \
    opencl-headers \
    opencl-clhpp \
    opencl-mesa
# Build image
#
FROM base as builder

USER root
RUN pacman -Syu --noconfirm --noprogressbar && \
    pacman -Sy --noconfirm --noprogressbar \
    base-devel \
    coreutils \
    git \
    go

RUN useradd -m builder
USER builder
WORKDIR /home/builder

RUN git clone https://aur.archlinux.org/mandelbulber2-opencl-git.git src && \
    cd src && \
    MAKEFLAGS="-j$(nproc)" makepkg


#
# Final image
#
FROM base as final

ENV MANDELBULBER=/usr/sbin/mandelbulber2 \
    USER_UID=1001 \
    USER_NAME=mandelbulber \
    USER_HOME=/home/mandelbulber

COPY --from=builder /home/builder/src/*.pkg.tar.zst /tmp/
RUN pacman -U /tmp/*.pkg.tar.zst  --noconfirm --noprogressbar && \
    rm -f /tmp/*.pkg.tar.zst

RUN useradd -m ${USER_NAME} -u ${USER_UID}

RUN mkdir -p /usr/share/doc/mandelbulber2
RUN chown "${USER_UID}:0" "${USER_HOME}" && \
    chmod ug+rwx "${USER_HOME}"

USER ${USER_NAME}
WORKDIR ${USER_HOME}
#ENTRYPOINT ["/usr/local/bin/entrypoint"]
#CMD ["--version"]

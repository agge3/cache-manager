FROM ubuntu

ARG DEBIAN_FRONTEND=noninteractive

ARG TZ=America/Los_Angles


RUN apt-get update && apt-get install -y linux-tools-common linux-tools-generic gcc git wget vim

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN ln -fs /usr/lib/linux-tools/*/perf /usr/bin/perf

RUN ln -fs /usr/lib/linux-tools/*/bpftool /usr/bin/bpftool

RUN ln -fs /usr/lib/linux-tools/*/rtla /usr/bin/rtla

WORKDIR /cm

COPY . /cm

ENTRYPOINT ["/cm/run.sh"]

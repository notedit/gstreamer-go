# gstreamer-go
gstreamer simple wrap for golang



## Install 


Ubuntu or Dedian

```
apt-get install libgstreamer1.0-0 gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav
```

Mac os

```
brew install gstreamer
brew install gst-plugins-base
brew install gst-plugins-good
brew install gst-plugins-bad
brew install gst-plugins-ugly
```


Then

```
go get github.com/notedit/gstreamer-go
```


## How to use

```
pipeline, err := New("videotestsrc  ! capsfilter name=filter ! autovideosink")
if err != nil {
    t.Error("pipeline create error", err)
    t.FailNow()
}

filter := pipeline.FindElement("filter")

if filter == nil {
    t.Error("pipeline find element error ")
}

filter.SetCap("video/x-raw,width=1280,height=720")

pipeline.Start()

```


## Send Media data to pipeline

```
pipeline, err := New("appsrc name=mysource format=time is-live=true do-timestamp=true ! videoconvert ! autovideosink")

if err != nil {
    t.Error("pipeline create error", err)
    t.FailNow()
}

appsrc := pipeline.FindElement("mysource")

appsrc.SetCap("video/x-raw,format=RGB,width=320,height=240,bpp=24,depth=24")

pipeline.Start()

for {
    time.Sleep(1 * time.Second)
    appsrc.Push(make([]byte, 320*240*3))
}

```

push raw rgb data to the pipeline


## Poll Media data from pipeline 

```
pipeline, err := New("videotestsrc ! video/x-raw,format=I420,framerate=15/1 ! x264enc bframes=0 speed-preset=veryfast key-int-max=60 ! video/x-h264,stream-format=byte-stream ! appsink name=sink")

if err != nil {
    t.Error("pipeline create error", err)
    t.FailNow()
}

appsink := pipeline.FindElement("sink")

pipeline.Start()

out := appsink.Poll()

for {
    buffer := <-out
    fmt.Println("push ", len(buffer))
}
```

now we can get the h264 raw data from the pipeline 





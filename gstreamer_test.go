package gstreamer

import (
	"fmt"
	"testing"
	"time"
)

func TestPipeline(t *testing.T) {

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

	select {}
}

func TestAppsrc(t *testing.T) {

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
}

func TestAppsink(t *testing.T) {

	pipeline, err := New("videotestsrc num-buffers=15 ! appsink name=sink")

	if err != nil {
		t.Error("pipeline create error", err)
		t.FailNow()
	}

	appsink := pipeline.FindElement("sink")

	pipeline.Start()

	out := appsink.Poll()

	for {
		buffer := <-out
		fmt.Println(len(buffer))
	}
}

func TestAppsink2(t *testing.T) {

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
}

func TestCheckPlugins(t *testing.T) {
	
	plugins := []string{"videotestsrc", "audiotestsrc", "rtp", "curl","x264", "rtmp"}

	err := CheckPlugins(plugins)

	if err != nil {
		t.Error("pipeline create error", err)
		t.FailNow()
	}
}

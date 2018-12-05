package gstreamer

import (
	"testing"
)

func TestPipeline(t *testing.T) {
	pipeline, err := New("videotestsrc ! capsfilter name=filter ! fakesink")
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
}

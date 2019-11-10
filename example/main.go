package main 

import "fmt"
import gstreamer "github.com/notedit/gstreamer-go"

func main() {
	generteVideoRTP(96,5000)
}

func generteVideoRTP( payload int, port int) {

	pipelineStr := "videotestsrc is-live=true ! video/x-raw,format=I420,framerate=15/1 ! x264enc aud=false bframes=0 speed-preset=veryfast key-int-max=15 ! video/x-h264,stream-format=byte-stream,profile=baseline ! h264parse ! rtph264pay config-interval=-1  pt=%d ! udpsink host=127.0.0.1 port=%d"
	
	pipelineStr = fmt.Sprintf(pipelineStr, payload, port)
	pipeline, err := gstreamer.New(pipelineStr)

	fmt.Println(pipelineStr)
	if err != nil {
		panic("can not create pipeline")
	}

	pipeline.Start()

	messages := pipeline.PullMessage()
	if messages == nil {
		panic("can not create pipeline")
	}
	
	for message := range messages {
		fmt.Println(message.GetTypeName())
	}
}


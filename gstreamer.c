

#include "gstreamer.h"
#include <gst/video/video.h>
#include <gst/gstcaps.h>



void gstreamer_init() {
    gst_init(NULL, NULL);
}


	// MESSAGE_UNKNOWN       MessageType = C.GST_MESSAGE_UNKNOWN
	// MESSAGE_EOS           MessageType = C.GST_MESSAGE_EOS
	// MESSAGE_ERROR         MessageType = C.GST_MESSAGE_ERROR
	// MESSAGE_TAG           MessageType = C.GST_MESSAGE_TAG
	// MESSAGE_BUFFERING     MessageType = C.GST_MESSAGE_BUFFERING
	// MESSAGE_STATE_CHANGED MessageType = C.GST_MESSAGE_STATE_CHANGED
	// MESSAGE_ANY           MessageType = C.GST_MESSAGE_ANY


typedef struct BusMessageUserData {
    int pipelineId;
} BusMessageUserData;


static gboolean gstreamer_bus_call(GstBus *bus, GstMessage *msg, gpointer user_data) {

    BusMessageUserData *s = (BusMessageUserData *)user_data;
    int pipelineId = s->pipelineId;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        goHandleBusMessage(msg,pipelineId);
        break;
    case GST_MESSAGE_ERROR: {
        gchar *debug;
        GError *error;
        gst_message_parse_error(msg, &error, &debug);
        g_free(debug);
        g_error_free(error);
        goHandleBusMessage(msg,pipelineId);
        break;
    }
    case GST_MESSAGE_BUFFERING: {
        goHandleBusMessage(msg,pipelineId);
        break;
    }
    case GST_MESSAGE_STATE_CHANGED: {
        goHandleBusMessage(msg,pipelineId);
        break;
    }
    
    default:
        break;
    }

    return TRUE;
}


GstPipeline *gstreamer_create_pipeline(char *pipelinestr) {
    GError *error = NULL;
    GstPipeline *pipeline = (GstPipeline*)GST_BIN(gst_parse_launch(pipelinestr, &error));
    return pipeline;
}


void gstreamer_pipeline_start(GstPipeline *pipeline, int pipelineId) {
    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
}


void gstreamer_pipeline_but_watch(GstPipeline *pipeline, int pipelineId) {
    BusMessageUserData *s = calloc(1, sizeof(BusMessageUserData));
    s->pipelineId = pipelineId;
    GstBus *bus = gst_pipeline_get_bus(pipeline);
    gst_bus_add_watch(bus, gstreamer_bus_call, s);
    gst_object_unref(bus);
}

void gstreamer_pipeline_pause(GstPipeline *pipeline) {
    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PAUSED);
}

void gstreamer_pipeline_stop(GstPipeline *pipeline) {
    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
}

void gstreamer_pipeline_sendeos(GstPipeline *pipeline) {
    gst_element_send_event(GST_ELEMENT(pipeline), gst_event_new_eos());
}

GstElement *gstreamer_pipeline_findelement(GstPipeline *pipeline, char *element) {
    GstElement *e = gst_bin_get_by_name(GST_BIN(pipeline), element);
    if (e != NULL) {
        gst_object_unref(e);
    }
    return e;
}

void gstreamer_pipeline_set_auto_flush_bus(GstPipeline *pipeline, gboolean auto_flush) {
    gst_pipeline_set_auto_flush_bus(pipeline, auto_flush);
}

gboolean gstreamer_pipeline_get_auto_flush_bus(GstPipeline *pipeline) {
    return gst_pipeline_get_auto_flush_bus(pipeline);
}

void gstreamer_pipeline_set_delay(GstPipeline *pipeline, GstClockTime delay) {
    gst_pipeline_set_delay(pipeline, delay);
}

GstClockTime gstreamer_pipeline_get_delay(GstPipeline *pipeline) {
    return gst_pipeline_get_delay(pipeline);
}

void gstreamer_pipeline_set_latency(GstPipeline *pipeline, GstClockTime latency) {
    gst_pipeline_set_latency(pipeline, latency);
}

GstClockTime gstreamer_pipeline_get_latency(GstPipeline *pipeline) {
    return gst_pipeline_get_latency(pipeline);
}


void gstreamer_set_caps(GstElement *element, char *capstr) {

    GObject *obj = G_OBJECT(element);
    GstCaps* caps = gst_caps_from_string(capstr);

    if (GST_IS_APP_SRC(obj)) {
        gst_app_src_set_caps(GST_APP_SRC(obj), caps);
    } else if (GST_IS_APP_SINK(obj)) {
        gst_app_sink_set_caps(GST_APP_SINK(obj), caps);
    } else {
        // we should make soure this obj have caps
        GParamSpec *spec = g_object_class_find_property(G_OBJECT_GET_CLASS(obj), "caps");
        if(spec) {
             g_object_set(obj, "caps", caps, NULL);
        } 
    }
    gst_caps_unref(caps);
}


void gstreamer_element_push_buffer(GstElement *element, void *buffer,int len) {
    gpointer p = g_memdup(buffer, len);
    GstBuffer *data = gst_buffer_new_wrapped(p, len);
    gst_app_src_push_buffer(GST_APP_SRC(element), data);
}


void gstreamer_element_push_buffer_timestamp(GstElement *element, void *buffer,int len, guint64 pts) {
    gpointer p = g_memdup(buffer, len);
    GstBuffer *data = gst_buffer_new_wrapped(p, len);
    GST_BUFFER_PTS(data) = pts;
    GST_BUFFER_DTS(data) = pts;
    gst_app_src_push_buffer(GST_APP_SRC(element), data);
}


typedef struct SampleHandlerUserData {
    int elementId;
    GstElement *element;
} SampleHandlerUserData;


GstFlowReturn gstreamer_new_sample_handler(GstElement *object, gpointer user_data) {
  GstSample *sample = NULL;
  GstBuffer *buffer = NULL;
  gpointer copy = NULL;
  gsize copy_size = 0;
  SampleHandlerUserData *s = (SampleHandlerUserData *)user_data;

  g_signal_emit_by_name (object, "pull-sample", &sample);
  if (sample) {
    buffer = gst_sample_get_buffer(sample);
    if (buffer) {
      gst_buffer_extract_dup(buffer, 0, gst_buffer_get_size(buffer), &copy, &copy_size);
      goHandleSinkBuffer(copy, copy_size, s->elementId);
    }
    gst_sample_unref (sample);
  }

  return GST_FLOW_OK;
}


GstFlowReturn gstreamer_sink_eos_handler(GstElement *object, gpointer user_data) {

    SampleHandlerUserData *s = (SampleHandlerUserData *)user_data;
    goHandleSinkEOS(s->elementId);
    return GST_FLOW_OK; 
}


void gstreamer_element_pull_buffer(GstElement *element, int elementId) {

    SampleHandlerUserData *s = calloc(1, sizeof(SampleHandlerUserData));
    s->element = element;
    s->elementId = elementId;

    g_object_set(element, "emit-signals", TRUE, NULL);
    g_signal_connect(element, "new-sample", G_CALLBACK(gstreamer_new_sample_handler), s);  
    g_signal_connect(element, "eos", G_CALLBACK(gstreamer_sink_eos_handler), s); 
}




#ifndef GST_H
#define GST_H

#include <glib.h>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>
#include <stdint.h>
#include <stdlib.h>


extern void goHandleSinkBuffer(void *buffer, int bufferLen, int elementId);
extern void goHandleBusMessage(GstMessage* message, int pipelineId);
extern void goHandleSinkEOS(int elementId);


static gint
toGstMessageType(void *p) {
	return (GST_MESSAGE_TYPE(p));
}

static const char*
messageTypeName(void *p)
{
	return (GST_MESSAGE_TYPE_NAME(p));
}

static guint64
messageTimestamp(void *p)
{
	return (GST_MESSAGE_TIMESTAMP(p));
}

void gstreamer_init();

GstPipeline *gstreamer_create_pipeline(char *pipeline);

void gstreamer_pipeline_start(GstPipeline *pipeline, int pipelineId);

void gstreamer_pipeline_but_watch(GstPipeline *pipeline, int pipelineId);

void gstreamer_pipeline_pause(GstPipeline *pipeline);

void gstreamer_pipeline_stop(GstPipeline *pipeline);

void gstreamer_pipeline_sendeos(GstPipeline *pipeline);

void gstreamer_pipeline_set_auto_flush_bus(GstPipeline *pipeline, gboolean auto_flush);

gboolean  gstreamer_pipeline_get_auto_flush_bus (GstPipeline *pipeline);

GstElement *gstreamer_pipeline_findelement(GstPipeline *pipeline, char *element);

void gstreamer_pipeline_set_delay(GstPipeline *pipeline, GstClockTime delay);

GstClockTime gstreamer_pipeline_get_delay(GstPipeline *pipeline);

void gstreamer_pipeline_set_latency(GstPipeline *pipeline, GstClockTime latency);

GstClockTime gstreamer_pipeline_get_latency(GstPipeline *pipeline);

void gstreamer_set_caps(GstElement *element, char *caps);

void gstreamer_element_push_buffer(GstElement *element, void *buffer,int len);

void gstreamer_element_push_buffer_timestamp(GstElement *element, void *buffer,int len, guint64 pts);

void gstreamer_element_pull_buffer(GstElement *element, int elementId);

#endif
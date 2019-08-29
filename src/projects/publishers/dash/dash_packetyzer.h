﻿//==============================================================================
//
//  OvenMediaEngine
//
//  Created by Hyunjun Jang
//  Copyright (c) 2019 AirenSoft. All rights reserved.
//
//==============================================================================
#pragma once

#include <segment_stream/packetyzer/m4s_init_writer.h>
#include <segment_stream/packetyzer/m4s_segment_writer.h>
#include <segment_stream/packetyzer/packetyzer.h>

enum class DashFileType : int32_t
{
	Unknown,
	PlayList,
	VideoInit,
	AudioInit,
	VideoSegment,
	AudioSegment,
};

class DashPacketyzer : public Packetyzer
{
public:
	DashPacketyzer(const ov::String &app_name, const ov::String &stream_name,
				   PacketyzerStreamType stream_type,
				   const ov::String &segment_prefix,
				   uint32_t segment_count, uint32_t segment_duration,
				   std::shared_ptr<MediaTrack> video_track, std::shared_ptr<MediaTrack> audio_track);

	static DashFileType GetFileType(const ov::String &file_name);

	virtual bool WriteVideoInit(const std::shared_ptr<ov::Data> &frame);
	virtual bool WriteAudioInit(const std::shared_ptr<ov::Data> &frame);

	virtual bool WriteVideoSegment();
	virtual bool WriteAudioSegment();

	//--------------------------------------------------------------------
	// Override Packetyzer
	//--------------------------------------------------------------------
	bool AppendVideoFrame(std::shared_ptr<PacketyzerFrameData> &frame) override;
	bool AppendAudioFrame(std::shared_ptr<PacketyzerFrameData> &frame) override;

	const std::shared_ptr<SegmentData> GetSegmentData(const ov::String &file_name) override;
	bool SetSegmentData(ov::String file_name, uint64_t duration, int64_t timestamp, std::shared_ptr<ov::Data> &data) override;

protected:
	static int GetStartPatternSize(const uint8_t *buffer);

	virtual ov::String GetFileName(int64_t start_timestamp, common::MediaType media_type) const;

	bool WriteVideoInitInternal(const std::shared_ptr<ov::Data> &frame, M4sTransferType transfer_type, const ov::String &init_file_name);
	bool WriteVideoInitIfNeeded(std::shared_ptr<PacketyzerFrameData> &frame);

	bool WriteAudioInitInternal(const std::shared_ptr<ov::Data> &frame, M4sTransferType transfer_type, const ov::String &init_file_name);
	bool WriteAudioInitIfNeeded(std::shared_ptr<PacketyzerFrameData> &frame);

	bool AppendVideoFrameInternal(std::shared_ptr<PacketyzerFrameData> &frame, uint64_t current_segment_duration, std::function<void(const std::shared_ptr<const SampleData> &data)> data_callback);
	bool AppendAudioFrameInternal(std::shared_ptr<PacketyzerFrameData> &frame, uint64_t current_segment_duration, std::function<void(const std::shared_ptr<const SampleData> &data)> data_callback);

	bool GetSegmentInfos(ov::String *video_urls, ov::String *audio_urls, double *time_shift_buffer_depth, double *minimum_update_period);

	virtual bool UpdatePlayList();

	int _avc_nal_header_size = 0;
	ov::String _start_time;
	std::string _pixel_aspect_ratio;
	double _mpd_min_buffer_time;

	double _ideal_duration_for_video = 0.0;
	double _ideal_duration_for_audio = 0.0;

	std::shared_ptr<SegmentData> _video_init_file = nullptr;
	std::shared_ptr<SegmentData> _audio_init_file = nullptr;

	uint32_t _video_sequence_number = 1;
	uint32_t _audio_sequence_number = 1;

	std::vector<std::shared_ptr<const SampleData>> _video_datas;
	int64_t _last_video_pts = -1LL;
	std::vector<std::shared_ptr<const SampleData>> _audio_datas;
	int64_t _last_audio_pts = -1LL;
	
	double _duration_margin;
};
/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cAnimAudioView - promoted QWidget to display the animation curve extracted from
 * the audio data and the audio settings
 */

#include "anim_audio_view.h"

#include <memory>

#include <QPainter>

#include "src/audio_track.h"

cAnimAudioView::cAnimAudioView(QWidget *parent) : QWidget(parent)
{
	playbackPositionX = 0;
	framesPerSecond = 30.0;
}

cAnimAudioView::~cAnimAudioView() = default;

void cAnimAudioView::UpdateChart(const std::shared_ptr<cAudioTrack> audioTrack)
{
	// Check if the audio track is valid and loaded
	if (audioTrack && audioTrack->isLoaded())
	{
		// Get information from the audio track
		const int numberOfFrames = audioTrack->getNumberOfFrames();

		// Frames per second for the audio track
		framesPerSecond = audioTrack->getFramesPerSecond();

		 // Set the width of the view to match the number of frames
		setFixedWidth(numberOfFrames);

		// Create an image to draw the audio waveform onto
		animAudioImage = QImage(QSize(numberOfFrames, height()), QImage::Format_RGB32);

		 // Fill the image with black
		animAudioImage.fill(Qt::black);

		// Use a QPainter to draw onto the image
		QPainter painter(&animAudioImage);

		// Determine the maximum Y-coordinate for drawing
		const int maxY = height() - 1;

		 // Start drawing from the bottom-left corner
		QPoint prevPoint(0, maxY);

		// Set pen color to green and enable antialiasing for smooth lines
		painter.setPen(Qt::green);
		painter.setRenderHint(QPainter::Antialiasing, true);

		// Iterate over each frame and draw the waveform
		for (int frame = 0; frame < numberOfFrames; frame++)
		{
			// Calculate the Y-coordinate based on the audio amplitude
			const QPoint point(frame, int(maxY - audioTrack->getAnimation(frame) * maxY));

			// Draw a line from the previous point to the current point
			painter.drawLine(prevPoint, point);

			// Update the previous point for the next iteration
			prevPoint = point;
		}

		// Trigger an update to redraw the view with the updated image
		update();
	}

	// If the audio track is not valid or not loaded
	else
	{
		// Clear the image and update the view
		animAudioImage = QImage();
		update();
	}
}

void cAnimAudioView::paintEvent(QPaintEvent *event)
{
	// Ignore the paint event object (not used in this function)
	Q_UNUSED(event);

	// Create a painter object to draw on the widget
	QPainter painter(this);

	// Draw the audio waveform image onto the widget at (0, 0)
	painter.drawImage(0, 0, animAudioImage);

	// Create a semi-transparent white pen for drawing the playback line
	const QPen pen(QColor(255, 255, 255, 128));

	// Set the painter's pen to the semi-transparent white pen
	painter.setPen(pen);

	// Draw a vertical line at the current playback position
	painter.drawLine(playbackPositionX, 0, playbackPositionX, height() - 1);
}

void cAnimAudioView::positionChanged(qint64 position)
{
	// Convert playback position from milliseconds to seconds
	const double time = position * 0.001;

	// Calculate the X-coordinate of the playback line based on time and frames per second
	playbackPositionX = int(time * framesPerSecond);

	// Trigger a repaint of the widget to update the playback line position
	update();
}

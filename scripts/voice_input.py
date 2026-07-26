#!/usr/bin/env python3
import argparse
import os
import subprocess
import sys
import tempfile


EXPECTED_FORMAT = (
    "task <name> at <YYYY-MM-DD_HH:MM> "
    "priority <high|medium|low> category <category> "
    "remind <YYYY-MM-DD_HH:MM>"
)


def log(message):
    print(message, file=sys.stderr)


def find_model_path():
    candidates = [
        "models/faster-whisper-tiny.en",
        "./models/faster-whisper-tiny.en",
        "../models/faster-whisper-tiny.en",
        "/home/code/projects/myschedule_project/models/faster-whisper-tiny.en",
    ]

    for path in candidates:
        if os.path.isdir(path):
            return path

    return None


def convert_audio_if_ffmpeg_exists(input_path):
    """
    Convert audio to 16kHz mono wav if ffmpeg is available.
    If ffmpeg is not available, use the original file directly.
    """
    try:
        subprocess.run(
            ["ffmpeg", "-version"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=True,
        )
    except Exception:
        log("[Info] ffmpeg not found. Using original audio file directly.")
        return input_path, None

    tmp = tempfile.NamedTemporaryFile(delete=False, suffix=".wav")
    tmp.close()

    cmd = [
        "ffmpeg",
        "-y",
        "-i",
        input_path,
        "-ar",
        "16000",
        "-ac",
        "1",
        tmp.name,
    ]

    result = subprocess.run(
        cmd,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )

    if result.returncode != 0:
        log("[Warning] ffmpeg conversion failed. Using original audio file directly.")
        try:
            os.remove(tmp.name)
        except OSError:
            pass
        return input_path, None

    return tmp.name, tmp.name


def recognize_with_whisper(audio_path):
    model_path = find_model_path()

    if model_path is None:
        log("[Error] faster-whisper model not found.")
        log("[Hint] Expected path: models/faster-whisper-tiny.en")
        return ""

    try:
        from faster_whisper import WhisperModel
    except Exception as e:
        log(f"[Error] faster-whisper import failed: {e}")
        return ""

    try:
        log(f"[Whisper] Loading model: {model_path}")

        model = WhisperModel(
            model_path,
            device="cpu",
            compute_type="int8",
            local_files_only=True,
        )

        log("[Whisper] Transcribing audio...")

        segments, info = model.transcribe(
            audio_path,
            language="en",
            beam_size=5,
            vad_filter=False,
        )

        texts = []
        for segment in segments:
            text = segment.text.strip()
            if text:
                texts.append(text)

        return " ".join(texts).strip()

    except Exception as e:
        log(f"[Error] faster-whisper recognition failed: {e}")
        return ""


def manual_mode():
    log("Expected recognized text format:")
    log(f"  {EXPECTED_FORMAT}")
    log("")
    log("[Manual Mode] Please input recognized text:")

    try:
        return input().strip()
    except EOFError:
        return ""


def file_mode(audio_file):
    log("Expected recognized text format:")
    log(f"  {EXPECTED_FORMAT}")
    log("")

    if not os.path.exists(audio_file):
        log(f"[Error] Audio file not found: {audio_file}")
        return ""

    log(f"[File Mode] Reading audio file: {audio_file}")

    converted_path, temp_path = convert_audio_if_ffmpeg_exists(audio_file)

    try:
        text = recognize_with_whisper(converted_path)
    finally:
        if temp_path is not None:
            try:
                os.remove(temp_path)
            except OSError:
                pass

    return text.strip()


def record_microphone(seconds):
    tmp = tempfile.NamedTemporaryFile(delete=False, suffix=".wav")
    tmp.close()

    cmd = [
        "arecord",
        "-d",
        str(seconds),
        "-f",
        "S16_LE",
        "-r",
        "16000",
        "-c",
        "1",
        tmp.name,
    ]

    log(f"[Mic Mode] Recording {seconds} seconds...")
    log("[Mic Mode] Please speak now.")

    result = subprocess.run(cmd)

    if result.returncode != 0:
        log("[Error] Microphone recording failed.")
        try:
            os.remove(tmp.name)
        except OSError:
            pass
        return None

    return tmp.name


def mic_mode(seconds):
    log("Expected recognized text format:")
    log(f"  {EXPECTED_FORMAT}")
    log("")

    audio_path = record_microphone(seconds)

    if audio_path is None:
        return ""

    try:
        text = recognize_with_whisper(audio_path)
    finally:
        try:
            os.remove(audio_path)
        except OSError:
            pass

    return text.strip()


def parse_args():
    parser = argparse.ArgumentParser(description="Voice input for myschedule")

    parser.add_argument(
        "--manual",
        action="store_true",
        help="Input text manually",
    )

    parser.add_argument(
        "--file",
        type=str,
        help="Recognize from audio file",
    )

    parser.add_argument(
        "--mic",
        action="store_true",
        help="Record from microphone",
    )

    parser.add_argument(
        "--seconds",
        type=int,
        default=6,
        help="Recording seconds for microphone mode",
    )

    return parser.parse_args()


def main():
    args = parse_args()

    if args.manual:
        text = manual_mode()
    elif args.file:
        text = file_mode(args.file)
    elif args.mic:
        text = mic_mode(args.seconds)
    else:
        text = mic_mode(args.seconds)

    if not text:
        log("[Error] Speech recognition returned empty text.")
        log("[Error] No recognized text.")
        return 1

    log(f"[Recognized] {text}")

    # Very important:
    # stdout only outputs recognized text for C++ program.
    print(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
